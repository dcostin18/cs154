/*
** cs154-2015 Project 5 ("p5ims") Instant Message Server
** basic.c: for functions that aren't called per-client-thread
*/

#include "ims.h"


/* call serverStop() upon getting "q" or "quit" on stdin */
void readQuitFromStdin(void) {
  char *line=NULL;
  size_t lsize=0;
  while (1) {
    ssize_t glret;
    if (verbose) {
      printf("Type \"q\" or \"quit\" to cleanly quit the server\n");
    }
    glret = getline(&line, &lsize, stdin);
    /* Ctrl-D or EOF will also break out of this loop */
    if (glret <= 0 || !strcmp("quit\n", line) || !strcmp("q\n", line)) {
      /* tell things to quit gracefully */
      free(line);
      quitting = 1;
      /* anything else to do here? */
      break;
    }

    /* ... else !strcmp("foo\n", line) to see if user typed "foo" and then
       return. You can use this to add your own additional commands here, like
       for querying globals or other aspects of server's internal state */

  }
  return;
}


int serverStart(impEm *iem) {
  static const char me[]="serverStart";

  if (verbose > 1) {
    printf("%s: hi\n", me);
  }
  global_db = malloc(sizeof(db_t));
  global_db->user = malloc(sizeof(dbUser_t));
  if (udbaseRead(iem)) {
    impEmAdd(iem, "%s: failed to read database file \"%s\"",
             me, udbaseFilename);
    return 1;
  }
  /* immediately try writing database, so that any errors here can be
     reported as a failure of server start-up. Whether and how you do
     error handling for subsequent calls to udbaseWrite() is up to you */
    if (udbaseWrite(iem)) {
      impEmAdd(iem, "%s: failed to write database file \"%s\"",
	       me, udbaseFilename);
      return 1;
    }
    pthread_t threadID;
    if(pthread_create(&threadID, NULL, udbaseReWrite, NULL)!=0){
      perror("Thread create error");
      //close(connectionDesc);
      //close(listenDesc);                                                             
      exit(1);
    }
  /* YOUR CODE HERE:
     -- create listening file descriptor for listenPort and listen() on it
     See http://beej.us/guide/bgnet/output/html/multipage/syscalls.html
     and May 18 2015 class slides
     -- start a thread to periodically save database
     -- figure out whether looking for "quit" on stdin should be done
     in a thread that is started here, or in main.c
  */
  //  while(1){
    //  pthread_t timerID;
    //if(pthread_create(&timerID, NULL, udbaseReWrite, NULL)!=0){
    //perror("Database Thread create error");
    //exit(1);
    //}     

  int clientfd;
  struct sockaddr_in server, client;

  //printf("Inside main, first uer is %s, size = %d\n", global_db->user[0].name, global_db->unum);
  
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd <0)
    printf("Failed to create socket\n");
  serverfd = sockfd;
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(listenPort);

  if(bind(sockfd, (struct sockaddr *)&server, sizeof(server))<0){
    printf("Bind failure");
    return 1;

  }
  listen(sockfd, 10);


  //pthread_t threadID;
  pthread_t threadID2;
  int size = sizeof(struct sockaddr_in);
  while((clientfd = accept(sockfd, (struct sockaddr *)&client, (socklen_t*)&size))){
    printf("Connection accepted\n");
    char* connection = impMsgToStr(NULL,
				  impMsgNew(iem, IMP_MSG_TYPE_ACK, 
					    IMP_OP_CONNECT, IMP_END));
    if(send(clientfd, connection, strlen(connection),0)<0){
      printf("Write error\n");
    }
    // char message[256];
    // if(recv(clientfd, message, 256,0)>0){
    //   char* con = impMsgToStr(NULL, impMsgNew(iem, IMP_MSG_TYPE_ACK, IMP_OP_LOGOUT, IMP_END));
    //   send(clientfd, con, strlen(con), 0);
    // }
    if(pthread_create(&threadID2, NULL, readClientRequest, &clientfd)!=0){
      perror("Thread create error");
      exit(1);
    }
    //  if(pthread_create(&threadID, NULL, udbaseReWrite, NULL)!=0){
    //perror("Thread create error");
    //close(connectionDesc);
    //close(listenDesc);
    //  exit(1);
    //}  
  }
  //}


  if (verbose) {
    printf("%s: server started on part %u from %s\n",
           me, listenPort, udbaseFilename);
  }
  if (verbose > 1) {
    printf("%s: bye\n", me);
  }
  return 0;
}

void serverStop(void) {
  static const char me[]="serverStop";

  if (verbose > 1) {
    printf("%s: hi\n", me);
  }


  /* ... YOUR CODE HERE. What needs to be done to clean up
     resources created during server execution? */


  if (verbose > 1) {
    printf("%s: bye\n", me);
  }
  return;
}

