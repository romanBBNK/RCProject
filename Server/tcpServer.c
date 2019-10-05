#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include "tcpServer.h"
#include "data.h"
#define BUFFERSIZE 1000

void question_get(int fd, int addrlen, int n, char *buffer, char *parse, int newfd){

	parse = strtok(NULL, " "); // parse = topic
	parse = strtok(NULL, "\n"); // parse = question / question_number

	memset(buffer, '\0', sizeof(char)*128);

	strcat(buffer, "QGR ");

	if ("no topic or question"){
		strcat(buffer, "EOF");
	} else if ("erro"){
		strcat(buffer, "ERR");
	} else {
		strcat(buffer, "qUserID");
		strcat(buffer, " ");
		strcat(buffer, "qsize");
		strcat(buffer, " ");
		strcat(buffer, "qdata");
		strcat(buffer, " ");
		strcat(buffer, "qIMG");
		if (strcmp("qIMG","1") == 0){
			strcat(buffer, " ");
			strcat(buffer, "qiext");
			strcat(buffer, " ");
			strcat(buffer, "qsize");
			strcat(buffer, " ");
			strcat(buffer, "qdata");
		}
		strcat(buffer, " ");
		strcat(buffer, "n_answers");//max 10 answers

		//for each answer
		strcat(buffer, " ");
		strcat(buffer, "aUserID");
		strcat(buffer, " ");
		strcat(buffer, "asize");
		strcat(buffer, " ");
		strcat(buffer, "adata");
		strcat(buffer, " ");
		strcat(buffer, "aIMG");
		if (strcmp("aIMG","1") == 0){
			strcat(buffer, " ");
			strcat(buffer, "aiext");
			strcat(buffer, " ");
			strcat(buffer, "AN");
			strcat(buffer, " ");
			strcat(buffer, "aisize");
			strcat(buffer, " ");
			strcat(buffer, "aidata");
		}
	}
	strcat(buffer, " \n");
	n=write(newfd,buffer,strlen(buffer));
	if(n==-1)
		exit(1);

	memset(buffer, '\0', sizeof(char)*128);
}

void question_submit(int fd, int addrlen, int n, char *buffer, char *parse, int newfd){

	parse = strtok(NULL, " "); // parse = qUserID
	parse = strtok(NULL, " "); // parse = topic
	parse = strtok(NULL, " "); // parse = question / question_number
	parse = strtok(NULL, " "); // parse = qsize
	parse = strtok(NULL, " "); // parse = qdata
	parse = strtok(NULL, " "); // parse = qIMG
	if (strcmp(parse,"1") == 0){
		parse = strtok(NULL, " "); // parse = iext
		parse = strtok(NULL, " "); // parse = isize
		parse = strtok(NULL, " "); // parse = idata
	}
	parse = strtok(NULL, "\n");
	memset(buffer, '\0', sizeof(char)*128);

	strcat(buffer, "QUR ");

	if ("question already exist"){
		strcat(buffer, "DUP");
	} else if ("questions list full"){
		strcat(buffer, "FUL");
	} else if ("erro"){
		strcat(buffer, "NOK");
	} else {
		strcat(buffer, "OK");
	}
	strcat(buffer, " \n");
	n=write(newfd,buffer,strlen(buffer));
	if(n==-1)
		exit(1);

	memset(buffer, '\0', sizeof(char)*128);
}

void answer_submit(int fd, int addrlen, int n, char *buffer, char *parse, int newfd){

	parse = strtok(NULL, " "); // parse = aUserID
	parse = strtok(NULL, " "); // parse = topic
	parse = strtok(NULL, " "); // parse = question / question_number
	parse = strtok(NULL, " "); // parse = asize
	parse = strtok(NULL, " "); // parse = adata
	parse = strtok(NULL, " "); // parse = aIMG
	if (strcmp(parse,"1") == 0){
		parse = strtok(NULL, " "); // parse = iext
		parse = strtok(NULL, " "); // parse = isize
		parse = strtok(NULL, " "); // parse = idata
	}
	parse = strtok(NULL, "\n");
	memset(buffer, '\0', sizeof(char)*128);

	strcat(buffer, "ANR ");
	if ("erro"){
		strcat(buffer, "ERR");
	} else if ("answers list full"){
		strcat(buffer, "FUL");
	} else if ("erro"){
		strcat(buffer, "NOK");
	} else {
		strcat(buffer, "OK");
	}
	strcat(buffer, " \n");
	n=write(newfd,buffer,strlen(buffer));
	if(n==-1)
		exit(1);
	memset(buffer, '\0', sizeof(char)*128);
}
