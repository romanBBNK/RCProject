#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include "tcp.h"
#define BUFFERSIZE 1000

void connectTCP(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr){
	n=connect(fd,res->ai_addr,res->ai_addrlen);
	if(n==-1)
		exit(1);
}

void question_get(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *topic, char *question) {
	connectTCP(fd, addrlen, n, res, addr);

	strcat(buffer, "GQU ");
	strcat(buffer, topic);
	strcat(buffer, " ");
	strcat(buffer, question);
	strcat(buffer, " \n");
	
	n = write(fd, buffer, BUFFERSIZE);
	if(n == -1)
		exit(1);

	memset(buffer, '\0', sizeof(char)*128);
	memset(topic, '\0', sizeof(char)*10);
	memset(question, '\0', sizeof(char)*10);

	n = read(fd,buffer, BUFFERSIZE);
	if(n == -1)
		exit(1);
	write(1, buffer, n);
}

void qg(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *topic, char *question_number) {
	connectTCP(fd, addrlen, n, res, addr);


	strcat(buffer, "GQU ");
	strcat(buffer, topic);
	strcat(buffer, " ");
	strcat(buffer, question_number);
	strcat(buffer, " \n");
	n = write(fd, buffer, BUFFERSIZE);
	if(n == -1)
		exit(1);
	memset(buffer, '\0', sizeof(char)*128);
	memset(topic, '\0', sizeof(char)*10);
	memset(question_number, '\0', sizeof(char)*10);

	n = read(fd, buffer, BUFFERSIZE);
	if(n == -1)
		exit(1);
	write(1, buffer, n);
}

void question_submit(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *parse, char *userID, 
		char *topic, char *question, char *textFile, char *imageFile){
	connectTCP(fd, addrlen, n, res, addr);

	strcat(buffer, "QUS ");
	strcat(buffer, userID);
	strcat(buffer, " ");
	strcat(buffer, topic);
	strcat(buffer, " ");
	strcat(buffer, question);
	strcat(buffer, " ");
	strcat(buffer, "itoa(sizeOfFile(textFile))");
	strcat(buffer, " ");
	strcat(buffer, "qdata");
	if (imageFile != NULL){
		strcat(buffer, " 1 ");
		strcat(buffer, "iext");
		strcat(buffer, " ");
		strcat(buffer, "isize");
		strcat(buffer, " ");
		strcat(buffer, "idata");
	} else {
		strcat(buffer, " 0");
	}
	strcat(buffer, " \n");

	n = write(fd, buffer, BUFFERSIZE);
	if(n == -1)
		exit(1);

	memset(buffer, '\0', sizeof(char)*128);
	memset(topic, '\0', sizeof(char)*10);
	memset(question, '\0', sizeof(char)*10);

	n = read(fd, buffer, BUFFERSIZE);
	if(n == -1)
		exit(1);

	parse = strtok(buffer, " ");
	parse = strtok(NULL, "\n");

	if ((strcmp(parse, "OK") == 0)) {
		write(1, "Topic  was successfully created\n", 32);
	} else if ((strcmp(parse, "DUP") == 0)) {
		write(1, "Topic  already exists\n", 22);
	} else if ((strcmp(parse, "FUL") == 0)) {
		write(1, "Topic list is full\n", 19);
	} else {
		write(1, "Topic has not been created\n", 27);
	}
}

void answer_submit(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *parse, char *userID, 
		char *topic, char *question, char *textFile, char *imageFile){
	connectTCP(fd, addrlen, n, res, addr);

	strcat(buffer, "ANS ");
	strcat(buffer, userID);
	strcat(buffer, " ");
	strcat(buffer, topic);
	strcat(buffer, " ");
	strcat(buffer, question);
	strcat(buffer, " ");
	strcat(buffer, "asize");
	strcat(buffer, " ");
	strcat(buffer, "adata");
	if (imageFile != NULL){
		strcat(buffer, " 1 ");
		strcat(buffer, "iext");
		strcat(buffer, " ");
		strcat(buffer, "isize");
		strcat(buffer, " ");
		strcat(buffer, "idata");
	} else {
		strcat(buffer, " 0");
	}
	strcat(buffer, " \n");

	n = write(fd, buffer, BUFFERSIZE);
	if(n == -1)
		exit(1);

	memset(buffer, '\0', sizeof(char)*128);
	memset(topic, '\0', sizeof(char)*10);
	memset(question, '\0', sizeof(char)*10);

	n = read(fd, buffer, BUFFERSIZE);
	if(n == -1)
		exit(1);

	parse = strtok(buffer, " ");
	parse = strtok(NULL, "\n");

	if ((strcmp(parse, "OK") == 0)) {
		write(1, "Topic  was successfully created\n", 32);
	} else if ((strcmp(parse, "FUL") == 0)) {
		write(1, "Topic list is full\n", 19);
	} else {
		write(1, "Topic has not been created\n", 27);
	}
}
