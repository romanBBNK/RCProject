#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include "udpServer.h"
#include "data.h"
#define BUFFERSIZE 10000

void validReg(int fd, int addrlen, int n, struct sockaddr_in addr, char *buffer, char *parse) {
	parse = strtok(NULL, "\n");

	if( 10000 < atoi(parse)  && atoi(parse) < 99999 ) {
		write(1,"user: ",6);
		write(1, parse, 6);
		write(1, "\n", 1);
		n=sendto(fd,"RGR OK\n",7,0,(struct sockaddr*) &addr, addrlen);
	}
	else {
		write(1,"user does not exist\n",20);
		n=sendto(fd,"RGR NOK\n",8,0,(struct sockaddr*) &addr, addrlen);
	}
	if(n==-1)
		exit(1);
}

void topic_list(int fd, int addrlen, int n, struct sockaddr_in addr, char *buffer, char count[], int topic_counter) {
	write(1,"List topics\n",12);
	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

    sprintf(count, "%d", topic_counter);
	strcat(buffer, "LTR ");

    strcat(buffer, count);

	getTopicList(buffer);
	strcat(buffer, "\n");

	n=sendto(fd,buffer,strlen(buffer),0,(struct sockaddr*) &addr, addrlen);
	if(n==-1)
		exit(1);
}

void topic_propose(int fd, int addrlen, int n, struct sockaddr_in addr, char *buffer, char *parse) {
	char *userID;
	userID = strtok(NULL, " "); // parse = userID
	parse = strtok(NULL, "\n"); // parse = topic

	int status = saveNewTopic(parse, userID);
	
	if (status == 0) {
		write(1, "Topic ", 6);
		write(1, parse, strlen(parse));
		write(1, " was successfully created by ", 29);
		write(1, userID, 5);
		write(1, "\n", 1);
		n=sendto(fd,"PTR OK\n",7,0,(struct sockaddr*) &addr, addrlen);
	} else if (status == 2) {
		write(1, "Topic ", 6);
		write(1, parse, strlen(parse));
		write(1, " already exists\n", 16);
		n=sendto(fd,"PTR DUP\n",8,0,(struct sockaddr*) &addr, addrlen);
	} else if (status == 1) {
		write(1, "Topic list is full\n", 19);
		n=sendto(fd,"PTR FUL\n",8,0,(struct sockaddr*) &addr, addrlen);
	} else {
		write(1, "Topic ", 6);
		write(1, parse, strlen(parse));
		write(1, " has not been created\n", 22);
		n=sendto(fd,"PTR NOK\n",8,0,(struct sockaddr*) &addr, addrlen);
	}
}

void question_list(int fd, int addrlen, int n, struct sockaddr_in addr, char *buffer, char *parse) {
	parse = strtok(NULL, "\n"); // parse = topic
	write(1,"List questions for topic: ",26);
	write(1, parse, strlen(parse));
	write(1,"\n",1);

	getLastQuestionList(buffer, parse);
	strcat(buffer, "\n");
	printf("%s", buffer);


	n=sendto(fd,buffer,strlen(buffer),0,(struct sockaddr*) &addr, addrlen);
	if(n==-1)
		exit(1);
}
