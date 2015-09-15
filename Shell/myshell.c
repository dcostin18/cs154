#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <ctype.h>

typedef struct sll sll;

struct sll{
  char* s;
  sll* next;
}; 

void sll_snoc(char *s, sll *ss)
{
  if(ss->next == NULL){
    sll* res = malloc(sizeof(sll));
    res->s = strdup(s);
    res->next = NULL;
    ss->next = res;
  }
  else
    sll_snoc(s, ss->next);
      
  return;
}

unsigned int sll_length(sll *ss)
{
  if(ss == NULL)
    return 0;
  else
    return 1 + sll_length(ss->next);
}

char** sll_to_array(sll* ss)
{
  int len = sll_length(ss);
  char** res = malloc(sizeof(char*)*(len+1));
  int i;
  sll* temp = ss;
  for(i = 0; i < len; i++){
    res[i] = temp->s;
    temp = temp->next;
  }
  res[i] = NULL;
  return res;
}

void sll_free(sll *ss)
{
  if(ss == NULL)
    return;
  else{
    sll_free(ss->next);
    free(ss);
    return;
  }
}

void sll_show(sll *ss)
{
  if(ss == NULL){
    printf("\n");
  }
  else if(ss->next == NULL){
    printf("%s\n", ss->s);
    return;
  }
  else{
    printf("%s, ", ss->s);
    return sll_show(ss->next);
  }
}

void snoc_test()
{
  sll* ss = malloc(sizeof(sll));
  ss->s = "a";
  ss->next = NULL;
  sll_snoc("b", ss);
  sll_snoc("c", ss);
  sll_snoc("d", ss);
  sll_show(ss);
  sll_free(ss);
}

void myPrint(char *msg)
{
  write(STDOUT_FILENO, msg, strlen(msg));
}

void myError()
{
  char error_message[30] = "An error has occurred\n";
  write(STDOUT_FILENO, error_message, strlen(error_message));
}

char* removeleadingspace(char* s)
{
  while(isspace(*s)&& *s!='\n')
    s++;
  return s;
}

void separate(char* toparse, char* delim, void f(char*))
{
  if(strlen(toparse)>512){
    myError();
    return;
  }
  char *rc, *saveptr;
  rc = strtok_r(toparse,delim, &saveptr);
  while(rc != NULL){
    f(removeleadingspace(rc));
    rc = strtok_r(NULL,delim, &saveptr); 
  }
}

sll* parse_args(char* str)
{
  sll* tor = malloc(sizeof(sll));
  char* saveptr;
  char* elt = strtok_r(removeleadingspace(str), " ", &saveptr);
  tor->s = strtok(elt, "\n");
  tor->next = NULL;
  while((elt = strtok_r(NULL, " ", &saveptr)))
    {
      if((elt = strtok(elt, "\n")))
  sll_snoc(removeleadingspace(elt), tor);
    }
  return tor;
}

int count_carrots(char* str)
{
  int len = strlen(str);
  int i;
  int count=0;
  for(i=0; i<len; i++){
    if(str[i]=='>')
      count++;
  }
  return count;
}

void outp(char* text, char* finame)
{
  int fd = open(finame, O_CREAT|O_WRONLY);
  if(fd!=-1)
  {
    write(fd, &text, sizeof(text));
    close(fd);
  }
  // FILE* file = fopen(finame, "ab+");
  // fprintf(file, "%s", text);
  // return;
  
}

/*int check_redir(char* command)
{
  
}*/

void execute(char *command)
{
  int n = count_carrots(command);
  if(n>1){
    myError();
    return;
  }
  //  char s[514];
  //strcpy(s,command);
  // char* c = strtok(s, " ");
  if(!strncmp(command,">",1)){
    myError();
    return;
  }
  command = strtok(command, ">");
  if(command==NULL)
    return;
  /*if(redir == 2){
    myError();
    return;
    }*/
  char* fi = strtok(NULL, " > ");
  // fi = removeleadingspace(fi);
  //  fi = strtok(fi, " ");
  //fi = strtok(fi, "\n");
  if(n == 1){    
    sll* tempfi = parse_args(fi);
    if(sll_length(tempfi) > 1){
      myError();
      sll_free(tempfi);
      return;
    }
    sll_free(tempfi);
  }
  int des = 1;
  if(fi!=NULL) {
    if(!strncmp(command, "exit",4)||
       !strncmp(command, "pwd",3) ||
       !strncmp(command, "cd",2)){
      myError();
      return;
    }
    removeleadingspace(fi);
    fi = strtok(fi, " ");
    fi = strtok(fi, "\n");
    des = open(fi, O_WRONLY | O_CREAT | O_EXCL, 0666);
    if(des==-1){
      myError();
      return;
    }
  }
  /*int bit = redirect(command);
  char* fname;
  if(bit == 2){
    myError();
    return;
  }
  else if(bit == 1)
  {
    // command to redirect
    command =strtok(command, ">");
    fname = strtok(NULL, ">");

    }*/
  if(!strncmp(command, "exit",4)){
    //        myPrint(command);
    sll* temp = parse_args(command);
    if(sll_length(temp)==1){
      sll_free(temp);
      exit(0);
    }
    else{
      sll_free(temp);
      myError();
      return;
    }
  }
  else if(!strncmp(command, " ",1)) {
    //separate(command, " ", execute);
    execute(removeleadingspace(command));
    return;
  }
  else if(!strncmp(command, "cd",2)) {    
    //myPrint(command);
    //myPrint("\n");
    sll* temp = parse_args(command);
    if(sll_length(temp)>2){
      myError();
      sll_free(temp);
      return;
    }
    else if(sll_length(temp) == 1 && 
      !strcmp(strtok(temp->s, "\n"),"cd")){
      chdir(getenv("HOME"));
      return;
    }
    //char* dir = strtok(command, " ");
    char*dir = temp->s;
    //    if(strtok(NULL, " ") == NULL){
    //myError();
    // return;
    //}
    if(strlen(dir)!=strlen("cd")&&strcmp(dir,"cd\n")){
      myError();
      return;
    }
    //dir = strtok(NULL, " ");
    dir = temp->next->s;
    char* dir2 = strtok(dir, "\n");
    if(dir2==NULL)
      dir2 = getenv("HOME");
    if(chdir(dir2))
      myError();
    return;
  }
  else if(!strcmp(command, "\n"))
    return;
  else{
    pid_t pid = fork();
    // child process executes command
    if(pid==0)
    {
        if(!strncmp(command, "pwd",3)){
    //        myPrint(command);
    //  myPrint("\n");
    sll* temp = parse_args(command);
    if(sll_length(temp) > 1){
      sll_free(temp);
      myError();
      exit(0);
    }
    sll_free(temp);
    char dir[514];
          //if(!bit)
    dup2(des, STDOUT_FILENO); 
            myPrint(getcwd(dir, 514));
      //else
      // outp(getcwd(dir, 514),fname);
            dup2(des, STDOUT_FILENO); 
            myPrint("\n");
      exit(0);
        }
  //        else if(!strncmp(command, " ",1)) {
  // separate(command, " ", execute);
  //}
  else{
    sll* list = parse_args(command);
    char** argz = sll_to_array(list);
    // myPrint(command);
          //myPrint("\n");
    dup2(des, STDOUT_FILENO); 
    if(execvp(argz[0], argz) == -1){
      myError();
      exit(0);
    }
    sll_free(list);
    free(argz);
  }
        //else
  //myError();
  dup2(des, STDOUT_FILENO); 
  myPrint("\n");
        exit(0);   
    }
  
  //parent process waits
    else
      {
        int ch;
    waitpid(pid, &ch, 0); 
        return;
      }
  }

}

void batch(char* fname)
{
    FILE* fp = fopen(fname, "r");
    if(fp==NULL)
    {
        myError();
        exit(0);
    }    
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    while((read = getline(&line, &len, fp))!=-1)
    {
      if(strcmp(removeleadingspace(line),"\n")){
        myPrint(line);
        separate(line, ";", execute);
  }
    }
    exit(0);
}

int main(int argc, char *argv[]) 
{
  char* v1=argv[1];
  if(v1!=NULL)
  {
      batch(v1);
  }
  //snoc_test();
  /*char** stuff;
    char topar[] = "Jonathan Michael Acevedo\n";
    int len = separate(&stuff, ' ', topar, 15);
    int i;
    printf("it at least didn't segfault there\n");
    for(i = 0; i < len; i++)
    printf("%s ", stuff[i]);
    printf("\n");
    free(stuff)*/
  char cmd_buff[250];
  char *pinput;
  while (1) {
    myPrint("myshell> ");
    //pinput2 = removeleadingspace(cmd_buff);  
    pinput = fgets(cmd_buff, 250, stdin);
      //      pinput2 = removeleadingspace(pinput);
      if (!pinput)
  exit(0);
      if(strcmp(removeleadingspace(pinput),"\n")){
      //    if(strcmp(argv[0],"\n")){
      myPrint(cmd_buff);
      //      myPrint("\n");
      separate(cmd_buff, ";", execute);
    }
    else{}
      /*int i;
      printf("%d\n",len);
      for(i = 0; i < len; i++)
      printf("%s ", stuff[i]);
      printf("\n");*/
    //len = separate(&stuff, ';', cmd_buff, 100);
    //for(i = 0; i < len; i++)
    // printf("%s ", stuff[i]);
    // printf("\n");
  }
}

// version 11
