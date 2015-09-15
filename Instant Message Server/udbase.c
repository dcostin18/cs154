/*
** cs154-2015 Project 5 ("p5ims") Instant Message Server
** udbase.c: for reading and writing the user database
*/

#include <fcntl.h>
#include "ims.h"

/* your in-memory representation of user database can be pointed to by some
   global variables, declared in ims.h, defined here, and initialized by
   udbaseRead below.  When server is running with multiple threads, access to
   these globals should be guarded by a mutex. */

int udbaseRead(impEm *iem) {
  static const char me[]="udbaseRead";
  FILE *file;

  file = fopen(udbaseFilename, "r");
  if (!file) {
    impEmAdd(iem, "%s: couldn't open \"%s\" for reading: %s",
             me, udbaseFilename, strerror(errno));
    return 1;
  }
  db_t* temp_db;
  if(!(temp_db = dbRead(file))){
    fprintf(stderr, "error");
    return 1;
  }
  /*else{
    global_db = (db_t *)malloc(sizeof(db_t));
    global_db->unum = temp_db->unum;
    // global_db->user = (dbUser_t *)malloc(global_db->unum*sizeof(dbUser_t));
    global_db->user = temp_db->user;
    //  printf("where is the segfault\n");
    //free(temp_db);
    }*/

  printf("HERE first user is %s\n", global_db->user[0].name);

  /* ... YOUR CODE HERE to read database file into memory. Assuming
     that this is called before the server starts taking connections
     from clients, this does not need to be thread-safe. */
  fclose(file);
  return 0;
}

void dbUserInit(dbUser_t *uu, unsigned int maxFriendNum) {
  uu->name = NULL;
  uu->fline = (char **)malloc(maxFriendNum*sizeof(char*));
  assert(uu->fline);
  uu->fnum = 0;
  uu-> login = 0;
  return;
}

void dbUserDone(dbUser_t *uu) {
  unsigned int fi;
  free(uu->name);
  for (fi=0; fi<uu->fnum; fi++) {
    free(uu->fline[fi]);
  }
  free(uu->fline);
  return;
}

FILE *fopener(const char *fname) {
  FILE *ret;
  if (!strcmp("-", fname)) {
    ret = stdin;
  } else {
    ret = fopen(fname, "r");
    if (!ret) {
      fprintf(stderr, "%s: couldn't open \"%s\" for reading: %s\n",
	      me, fname, strerror(errno));
    }
  }
  return ret;
}

void fcloser(FILE *ff) {
  if (ff && stdin != ff) {
    fclose(ff);
  }
  return;
}

int getliner(char **pline, size_t *psize, FILE *file,
             ssize_t wantlen) {
  ssize_t red;

  red = getline(pline, psize, file);
  if (red <= -1) {
    fprintf(stderr, "%s: hit EOF\n", me);
    return 1;
  }
  if (red <= 1) {
    fprintf(stderr, "%s: got empty line\n", me);
    return 1;
  }
  if ('\n' != (*pline)[red-1]) {
    fprintf(stderr, "%s: line didn't end with '\\n'\n", me);
    return 1;
  }
  (*pline)[red-1] = '\0';
  if (!( red-1 >= wantlen )) {
    fprintf(stderr, "%s: got line \"%s\" with %u chars before \\n, "
            "but wanted >= %u\n", me, *pline, (unsigned int)(red-1),
            (unsigned int)wantlen);
    return 1;
  }
  return 0;
}

int userCompare(const void *_aa, const void *_bb) {
  dbUser_t *aa = (dbUser_t *)_aa;
  dbUser_t *bb = (dbUser_t *)_bb;
  return strcmp(aa->name, bb->name);
}

int lineCompare(const void *_aa, const void *_bb) {
  char *aa = *((char **)_aa);
  char *bb = *((char **)_bb);
  return strcmp(aa, bb);
}

db_t *dbRead(FILE *ff) {
  //  db_t *db;
  char *line, tbuf[128];
  size_t lsize;

  lsize = 0;
  line = NULL;
  if (getliner(&line, &lsize, ff, 8)) {
    fprintf(stderr, "%s: couldn't read first line\n", me);
    return NULL;
  }
  assert(line);
  unsigned int nuser;
  if (1 != sscanf(line, "%u users:", &nuser)) {
    fprintf(stderr, "%s: couldn't parse \"N users\" from first "
            "line \"%s\"\n", me, line);
    return NULL;
  }
  sprintf(tbuf, "%u users:", nuser);
  if (strcmp(tbuf, line)) {
    fprintf(stderr, "%s: first line \"%s\" more than \"%s\"\n",
            me, line, tbuf);
    return NULL;
  }
  //global_db = (db_t *)malloc(sizeof(db_t));
  //assert(db);
  global_db->unum = nuser;
  //free(global_db->user);
  global_db->user = (dbUser_t *)malloc(nuser*sizeof(dbUser_t));
  if (!global_db->user) {
    fprintf(stderr, "%s: failed to allocate %u users\n", me, nuser);
    return NULL;
  }
  unsigned int ui, li;
  for (ui=0; ui<nuser; ui++) {
    if (getliner(&line, &lsize, ff, 1)) {
      fprintf(stderr, "%s: couldn't read username %u/%u\n", me, ui, nuser);
      return NULL;
    }
    dbUserInit(global_db->user + ui, nuser);
    global_db->user[ui].name = strdup(line);
    printf("%s\n",global_db->user[ui].name);
    li = 0;
    do {
      if (getliner(&line, &lsize, ff, 1)) {
        fprintf(stderr, "%s: couldn't read friend line %u of user %u (%s)\n",
                me, li, ui, global_db->user[ui].name);
        return NULL;
      }
      if (strcmp(".", line)) {
        if (!(strlen(line) >= 3 && '-' == line[0] && ' ' == line[1])) {
          fprintf(stderr, "%s: user %u (%s) friend line %u \"%s\" should "
                  "start with \"- \" with strlen >= 3 (not %u)\n",
                  me, ui, global_db->user[ui].name, li, line,
                  (unsigned int)strlen(line));
          return NULL;
        }
        global_db->user[ui].fline[li] = strdup(line);
      }
      li++;
    } while (strcmp(".", line));
    global_db->user[ui].fnum = li-1;
    qsort(global_db->user[ui].fline, li-1, sizeof(char*), lineCompare);
  }
  qsort(global_db->user, nuser, sizeof(dbUser_t), userCompare);
  free(line);
  return global_db;
}

void dbFree(db_t *db) {
  unsigned int ui;
  for (ui=0; ui<db->unum; ui++) {
    dbUserDone(db->user + ui);
  }
  free(db->user);
  free(db);
}

void dbAddUser(db_t *db, dbUser_t *newUser) 
{
  printf("Beginning to add user...\n");
  printf("First user is %s\n", db->user[0].name);
  int i;
  db_t *temp_db = (db_t*)malloc(sizeof(db_t));
  temp_db->unum = global_db->unum + 1;
  temp_db->user = (dbUser_t *)malloc(temp_db->unum*sizeof(dbUser_t));
  //printf("segfault here?\n");
  printf("[%s]\n",global_db->user[global_db->unum-2].name);
  for(i = 0; i < global_db->unum; i++){
    temp_db->user[i] = global_db->user[i];
    printf("[%s] from [%s]\n",temp_db->user[i].name,global_db->user[i].name);
  }
  //printf("segfault here??\n");
  //free(global_db);
  // free(global_db->user);
  global_db = (db_t*)malloc(sizeof(db_t));
  //global_db->user = temp->user;
  //free(temp);
  //dbUser_t newlist[global_db->unum]; //malloc(sizeof(dbUser_t)* global_db->unum);
  //for(i = 0; i < global_db->unum - 1; i++)
  //newlist[i] = db->user[i];
  //free(global_db->user);
  // global_db->user = newlist;
  //global_db->user = 
  //(dbUser_t*) realloc(global_db->user, sizeof(dbUser_t)* global_db->unum);
  //  if(temp)
  //global_db->user = temp;
  //else
  //printf("error mah boy, error!");
  temp_db->user[temp_db->unum - 1] = *newUser;
  printf("segfault here???\n");
  //for(i = 0; i < global_db->unum; i++){
    
  // temp_db->user[i].fline = (char **)malloc(temp_db->unum*sizeof(char*));
  printf("segfault here????\n");
  global_db->user = temp_db->user;
  int ui;
  int size = temp_db->unum;
  for(ui = 0; ui < size; ui++){
    printf("next name = %s\n", temp_db->user[ui].name);
  }
  global_db->unum = temp_db->unum;
  printf("%d\n",global_db->unum);
  udbaseWrite(NULL);
  return;
}

/* you can pass a NULL iem to this if you aren't interested in saving the
   error messages; impEmAdd will have no effect with a NULL iem */
int udbaseWrite(impEm *iem) {
  printf("%d\n", global_db->unum);
  printf("pls\n");
  printf("%s\n", global_db->user[global_db->unum-1].name);
  static const char me[]="udbaseWrite";
  FILE *file;

  /* ... make sure that user database is being written at the same
     that a client thread is modifying it, either with code here,
     or with limits on how udbaseWrite() is called */

  file = fopen(udbaseFilename, "w");
  if (!file) {
    impEmAdd(iem, "%s: couldn't open \"%s\" for writing: %s",
             me, udbaseFilename, strerror(errno));
    return 1;
  }

  /* ... YOUR CODE HERE to write the in-memory database to file */
  //int saved_stdout = dup(1);
  //int des = open(udbaseFilename, O_CREAT | O_WRONLY, 0666);
  printf("des =, unum = %d\n", global_db->unum);
  //dup2(des, STDOUT_FILENO);
  unsigned int ui, li;
  fprintf(file, "%u users:\n", global_db->unum);
  for(ui = 0; ui<global_db->unum; ui++){
    fprintf(file, "%s\n", global_db->user[ui].name);
    for(li=0; li<global_db->user[ui].fnum; li++){
      fprintf(file,"%s\n", global_db->user[ui].fline[li]);
    }
    fprintf(file,".\n");
  }
  //dbFree(global_db);
  //dup2(saved_stdout, 1);
  //close(saved_stdout);

  fclose(file);
  return 0;
}

void* udbaseReWrite(void *iem)
{
  // if(write(clientfd,"ACK CONNECT\0", 12)<0){
  //   printf("Write failed\n");
  // }
  while(1){
    //  printf("Three seconds have passed...\n");
    sleep(saveInterval);
    //printf("save interval is: %d\n",saveInterval);
    //udbaseRead(NULL);
    //printf("%d\n", global_db->unum);
    //printf("this should show alice: %s\n",global_db->user[0].name);
    udbaseWrite(NULL);
    printf("Rewritten\n");
    // write(PARMPTR->connectionDesc, "Rewritten\0", 10);
  }
}

int isInGlobalDB(char* username)
{
  int i;
  for(i = 0; i < global_db->unum; i++)
    if(!strcmp(username, global_db->user[i].name))
      return 1;
  return 0;
}

//pre user with username must exist in globaldb, otherwise return bogus 0
int isActive(char* username)
{
  int i;
  for(i = 0; i < global_db->unum; i++){
    if(!strcmp(username, global_db->user[i].name))
      return global_db->user[i].login;
  }
  return 0;
}

// pre: user must exist in globaldb
void userLogin(char* username, int clientfd)
{
  int i;
  for(i = 0; i < global_db->unum; i++){
    if(!strcmp(username, global_db->user[i].name)){
      global_db->user[i].login=1;
      global_db->user[i].filedes = clientfd;
      return;
    }
  }
  return;
}

void updateFriends(dbUser_t* user)
{ 
  int i, x;
  for(i=0; i<user->fnum; i++)
  {
    char* frnd = user->fline[i];
    frnd+=2;
    char* one = strtok(frnd, " ");
    char* two = strtok(NULL, " ");
    for(x=0; x<global_db->unum; x++){
      if(!strcmp(frnd, global_db->user[x].name)){
      char* update;
      // if are friends
      if(!two){
        if(isActive(one)){
          update = impStrNew(NULL, IMP_MSG_TYPE_STATUS, one, IMP_FRIEND_YES, IMP_ACTIVE_NOT, IMP_END);
          send(global_db->user[x].filedes, update, strlen(update), 0);
        }
      }
    }
  }
  }
}

void updateAboutFriends(dbUser_t* user, int clientfd)
{
  int i, x;
  for(i=0; i<user->fnum; i++)
  {
    char* frnd = user->fline[i];
    frnd+=2;
    char* one = strtok(frnd, " ");
    char* two = strtok(NULL, " ");
    printf("one = %s\n", one);
    printf("two = %s\n", two);
    for(x = 0; x < global_db->unum; x++){
      if(!strcmp(frnd, global_db->user[x].name)){
        char* update;
        if(!two){
          if(isActive(one)){
          update = impStrNew(NULL, IMP_MSG_TYPE_STATUS, one, IMP_FRIEND_YES, IMP_ACTIVE_YES, IMP_END);
          send(clientfd, update, strlen(update), 0);
          }
          else{
          update = impStrNew(NULL, IMP_MSG_TYPE_STATUS, one, IMP_FRIEND_YES, IMP_ACTIVE_NOT, IMP_END);
          send(clientfd, update, strlen(update), 0);            
          }
        } 
        else{
          update = impStrNew(NULL, IMP_MSG_TYPE_STATUS, one, IMP_FRIEND_NOT, IMP_ACTIVE_NOT, IMP_END);
          send(clientfd, update, strlen(update), 0);                   
        }
      
    }

    
  }
  }
  return;
}

int isFriendOf(char* me, char* person)
{
  int i;
  dbUser_t* usr = NULL;
  for(i=0; i<global_db->unum; i++)
  {
    if(!strcmp(me, global_db->user[i].name)){
      usr = &global_db->user[i];
    }
  }
  for(i=0; i<usr->fnum; i++){
    char* temp = usr->fline[i];
    temp+=2;
    if(!strncmp(person, usr->fline[i], strlen(person))){
      return 1;
    }
  }
  return 0;
}

void* readClientRequest(void* input)
{
  char* active_user = NULL;
  // char receive[15];
  //if(recv(serverfd, &receive,12,0))
  //printf("rec error\n");
  //printf("%s",receive);
  int* temp = (int*) input;
  int clientfd = *temp;
  char buf[256];
  // int msglen;
  //  int i;
  // while((msglen = read(clientfd, &buf, 256))>0){
  while(1){
    int read = recv(clientfd, buf, 256, 0);
    buf[read-1] = '\0';
    impMsg* imp_mess = impStrToMsg(NULL, buf);
    impMsgOp* op_mess = (impMsgOp*) imp_mess;
    char *uu=NULL;
    char *error, *message;
    switch(op_mess->op){

      case IMP_OP_REGISTER:
        uu = op_mess->userName;
        if(isInGlobalDB(uu)){
          error = impStrNew(NULL, IMP_MSG_TYPE_ERROR, IMP_ERROR_USER_EXISTS, uu, IMP_END);
          send(clientfd, error, strlen(error), 0);
        }
        else if(active_user){
          error = impStrNew(NULL, IMP_MSG_TYPE_ERROR, IMP_ERROR_CLIENT_BOUND, active_user, IMP_END);
          send(clientfd, error, strlen(error), 0);
        }
        else if(strlen(uu)>IMP_NAME_MAXLEN || strlen(uu)<1){
          error = impStrNew(NULL, IMP_MSG_TYPE_ERROR, IMP_ERROR_BAD_COMMAND, IMP_END);
          send(clientfd, error, strlen(error), 0);
        }
        else{
          dbUser_t* user = malloc(sizeof(dbUser_t));
          dbUserInit(user, global_db->unum);
          user->name = uu;
          dbAddUser(global_db, user);
          message = impStrNew(NULL, IMP_MSG_TYPE_ACK, IMP_OP_REGISTER, uu, IMP_END);
          send(clientfd, message, strlen(message), 0);
        }
        break;

      case IMP_OP_LOGIN:
        uu = op_mess->userName;
        if(active_user){
          error = impStrNew(NULL, IMP_MSG_TYPE_ERROR, IMP_ERROR_CLIENT_BOUND, active_user, IMP_END);
          send(clientfd, error, strlen(error), 0);
        }
        else if(!isInGlobalDB(uu)){
          error = impStrNew(NULL, IMP_MSG_TYPE_ERROR, IMP_ERROR_USER_DOES_NOT_EXIST, IMP_END);
          send(clientfd, error, strlen(error), 0);
        }
        else if(isActive(uu)){
          error = impStrNew(NULL, IMP_MSG_TYPE_ERROR, IMP_ERROR_USER_ALREADY_ACTIVE, uu, IMP_END);
          send(clientfd, error, strlen(error), 0);          
        }
        else{
          active_user = uu;
          userLogin(uu, clientfd);
          message = impStrNew(NULL, IMP_MSG_TYPE_ACK, IMP_OP_LOGIN, uu, IMP_END);
          send(clientfd, message, strlen(message), 0);
          int i;
          dbUser_t* usr=NULL;
          for(i=0; i<global_db->unum; i++)
          {
            if(!strcmp(global_db->user[i].name, uu))
              usr = &global_db->user[i];
          }
          updateAboutFriends(usr, clientfd);
        }
        break;

      case IMP_OP_LOGOUT:
        uu = op_mess->userName;
        if(!active_user){
          error = impStrNew(NULL, IMP_MSG_TYPE_ERROR, IMP_ERROR_CLIENT_NOT_BOUND, IMP_END);
          send(clientfd, error, strlen(error), 0);          
        }
        else{
          message = impStrNew(NULL, IMP_MSG_TYPE_ACK, IMP_OP_LOGOUT, IMP_END);
          send(clientfd, message, strlen(message), 0); 
          int i;
          for(i=0; i<global_db->unum; i++){
            if(!strcmp(uu, global_db->user[i].name)){
              global_db->user[i].login=0;
              updateFriends(&global_db->user[i]);
            }
          }  


        }
        break;

      case IMP_OP_FRIEND_REQUEST:
        uu = op_mess->userName;
        if(!isInGlobalDB(uu)){
          error = impStrNew(NULL, IMP_MSG_TYPE_ERROR, IMP_ERROR_USER_DOES_NOT_EXIST, IMP_END);
          send(clientfd, error, strlen(error), 0);   
        }
        if(!strcmp(uu, active_user)){
          error = impStrNew(NULL, IMP_MSG_TYPE_ERROR, IMP_ERROR_BAD_COMMAND, IMP_END);
          send(clientfd, error, strlen(error), 0);             
        }
        if(isFriendOf(active_user, uu)){
          error = impStrNew(NULL, IMP_MSG_TYPE_ERROR, IMP_ERROR_FRIEND_ALREADY, IMP_END);
          send(clientfd, error, strlen(error), 0);            
        } 
        if(active_user== NULL){
          error = impStrNew(NULL, IMP_MSG_TYPE_ERROR, IMP_ERROR_CLIENT_NOT_BOUND, IMP_END);
          send(clientfd, error, strlen(error), 0);              
        }
        break;

      case IMP_OP_IM:
        uu = op_mess->userName;
        if(!isInGlobalDB(uu)){
          error = impStrNew(NULL, IMP_MSG_TYPE_ERROR, IMP_ERROR_USER_DOES_NOT_EXIST, IMP_END);
          send(clientfd, error, strlen(error), 0);              
        }
        if(!isFriendOf(active_user, uu)){
          error = impStrNew(NULL, IMP_MSG_TYPE_ERROR, IMP_ERROR_NOT_FRIEND, IMP_END);
          send(clientfd, error, strlen(error), 0);              
        }
        if(!isActive(uu)){
          error = impStrNew(NULL, IMP_MSG_TYPE_ERROR, IMP_ERROR_USER_NOT_ACTIVE, uu, IMP_END);
          send(clientfd, error, strlen(error), 0);              
        }
        if(active_user== NULL){
          error = impStrNew(NULL, IMP_MSG_TYPE_ERROR, IMP_ERROR_CLIENT_NOT_BOUND, IMP_END);
          send(clientfd, error, strlen(error), 0);              
        }
        break;


      case IMP_OP_FRIEND_REMOVE:
        uu = op_mess->userName;
        if(!isFriendOf(active_user, uu)){
          error = impStrNew(NULL, IMP_MSG_TYPE_ERROR, IMP_ERROR_NOT_FRIEND, uu, IMP_END);
          send(clientfd, error, strlen(error), 0);           
        }
        if(!strcmp(uu, active_user)){
          error = impStrNew(NULL, IMP_MSG_TYPE_ERROR, IMP_ERROR_BAD_COMMAND, IMP_END);
          send(clientfd, error, strlen(error), 0);             
        }     
        if(active_user== NULL){
          error = impStrNew(NULL, IMP_MSG_TYPE_ERROR, IMP_ERROR_CLIENT_NOT_BOUND, IMP_END);
          send(clientfd, error, strlen(error), 0);              
        }        
        break;   


      default:
        break;




    }
  }
  return input;
}


 //    //buf[msglen] = '\0';
 //    if(!strncmp("login", buf, 5)){
 //      return input;
 //    }
 //    else if(!strncmp("register", buf, 8)){
 //      printf("here\n");
 //      // char* command = impMsgToStr(NULL,
 //      //			  impMsgNew(NULL, IMP_MSG_TYPE_OP,
 //      //				    IMP_OP_REGISTER,
 //      //				    temp, IMP_END));
 //      //if (udbaseRead(NULL)) {
 //      //impEmAdd(iem, "%s: failed to read database file \"%s\"",                     
 //      //       me, udbaseFilename);                                                  
 //      //printf("error\n");
 //      //return input;
 //      //}
 //      //      for(i = 0; i < global_db->unum; i++)
 //   //printf("%s\n", global_db->user[i].name);
 //      char* temp = buf + 9;
 //      temp = strtok(temp,"\n");
 //      temp[strlen(temp)-1] = '\0'; 
 //      printf("%s\n",temp);
 //      if(active_user){
	// char* error = impMsgToStr(NULL, 
	// 			  impMsgNew(NULL, 
	// 				    IMP_MSG_TYPE_ERROR, 
	// 				    IMP_ERROR_CLIENT_BOUND, 
	// 				    active_user, 
	// 				    IMP_END));
	// send(clientfd, error, strlen(error),0);
 //      }
 //      else if(strlen(temp)>IMP_NAME_MAXLEN || strlen(temp) < 1){
	// char* error = impMsgToStr(NULL,
	// 			  impMsgNew(NULL,
	// 				    IMP_MSG_TYPE_ERROR,
	// 				    IMP_ERROR_BAD_COMMAND,
	// 				    IMP_END));
	// send(clientfd, error, strlen(error),0);
 //      }
 //      else if(isInGlobalDB(temp)){
	// char* error = impMsgToStr(NULL,
	// 			  impMsgNew(NULL,
	// 				    IMP_MSG_TYPE_ERROR,
 //                                            IMP_ERROR_USER_EXISTS,
	// 				    temp,
	// 				    IMP_END));
	// send(clientfd, error, strlen(error),0);
 //      }
 //      else{
	// printf("else entered\n");
	// char* command = impMsgToStr(NULL,
	// 			    impMsgNew(NULL, IMP_MSG_TYPE_OP,
	// 				      IMP_OP_REGISTER,
	// 				      temp, IMP_END));
	// printf("about to send to server\n");
	// //if(send(serverfd,command,strlen(command),0);
	// printf("command parsed: %s\n", command);
	// //uu->name = strtok(temp,"\n");
	// //uu->name[strlen(uu->name)-1] = '\0';
	// //if(!isInGlobalDB(temp)){
	// dbUser_t* uu = malloc(sizeof(dbUser_t));
	// dbUserInit(uu, global_db->unum);
	// uu->name = temp;
	// //printf("Look here!![%s]\n",uu->name);
	// //if(write(clientfd, temp, strlen(buf))<0)
	// //printf("Write error\n");
	// /*char* tempstr = buf+8;
	//   char* name;
	//   sscanf(
	//   uu->name = */
	// printf("Before the call, first user is %s, unum = %d\n", global_db->user[0].name, global_db->unum);
	// dbAddUser(global_db, uu);
	// char* registration=impMsgToStr(NULL,
	// 			       impMsgNew(NULL, IMP_MSG_TYPE_ACK, 
	// 					 IMP_OP_REGISTER,
	// 					 temp, IMP_END)); 
	// printf("going to send regis\n");
	// if(send(clientfd, registration, strlen(registration), 0))
	//   printf("Write error\n");
	// printf("%s\n",global_db->user[global_db->unum - 1].name);
	// if (udbaseWrite(NULL)) {
	//   //impEmAdd(iem, "%s: failed to read database file \"%s\"",
	//   //	   me, udbaseFilename);
	//   printf("error\n");
	//   return input;
	// }
	// printf("%s\n",global_db->user[global_db->unum - 1].name);
 //      }
 //      //else{
 //      //if(write(clientfd, "ERROR USER_EXISTS <%s>\n\0", 22+strlen(temp)))
 //      //  printf("Write error\n");
 //      //}
 //    }
 //    //else{
 //    // printf("jonathan put stuff here for bad command");
 //    //}
 //    else if(write(clientfd, buf, strlen(buf))<0)
 //      printf("Write error\n");
 //  }
 //  return input;


