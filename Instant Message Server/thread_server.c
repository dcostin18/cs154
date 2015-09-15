#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include "cndb.h"

#define PORTNUMBER 15400
#define PARMPTR ((struct serverParm *) parmPtr)

const char *me;
char *dbfile, *outfile;

struct serverParm{
	int connectionDesc;
};

void *thread2(void *parmPtr)
{
	while(1){
	printf("Three seconds have passed...\n");
	db_t* db = file_to_db(dbfile);
    db_to_file(db, outfile);
    printf("Rewritten\n");
    write(PARMPTR->connectionDesc, "Rewritten\0", 10);
    sleep(3);
}

}

void *thread(void *parmPtr)
{
	printf("I am here\n");
	while(1){
	printf("Three seconds have passed...\n");
	db_t* db = file_to_db(dbfile);
    db_to_file(db, outfile);
    printf("Rewritten\n");
    write(PARMPTR->connectionDesc, "Rewritten\0", 10);
    sleep(3);
}
	// int receivedMsgLen;
	// char messageBuf[1025];

	// // receive messages from sender
	// while((receivedMsgLen=read(PARMPTR->connectionDesc, messageBuf, sizeof(messageBuf)-1))>0)
	// {
	// 	receivedMsgLen[messageBuf] = '\0';
	// 	printf("Message: %s\n", messageBuf);
	// 	if(write(PARMPTR->connectionDesc, "GOT IT\0", 7)<0){
	// 		perror("Server: write error");
	// 		return(0);
	// 	}
	// }
	// close(PARMPTR->connectionDesc);
	// free(PARMPTR);
	// return(0);
}

int main(int argc, char** argv)
{
	int listenDesc;
	struct sockaddr_in myAddr;
	struct serverParm *parmPtr;
	int connectionDesc;
	pthread_t threadID;

	dbfile = argv[1];
	outfile = argv[2];
	// create socket from which to read
	if((listenDesc = socket(AF_INET, SOCK_STREAM, 0))<0){
		perror("open error on socket");
		exit(1);
	}
	// create name of socket
	myAddr.sin_family = AF_INET;
	myAddr.sin_addr.s_addr = INADDR_ANY;
	myAddr.sin_port = htons(PORTNUMBER);

	if(bind(listenDesc, (struct sockaddr *) &myAddr, sizeof(myAddr))<0){
		perror("bind error");
		exit(1);
	}
	listen(listenDesc, 5);

	while(1){
		// wait for client connection
		connectionDesc = accept(listenDesc, NULL, NULL);
		// create thread to handle client
		parmPtr = (struct serverParm *)malloc(sizeof(struct serverParm));
		parmPtr->connectionDesc = connectionDesc;
		if(pthread_create(&threadID, NULL, thread2, (void *)parmPtr)!=0){
			perror("Thread create error");
			close(connectionDesc);
			close(listenDesc);
			exit(1);
		}
		printf("Parent ready for another ocnnection");
	}

}
// Version 1