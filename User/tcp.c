#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <math.h>
#include "tcp.h"
#define BUFFERSIZE 1000

int sizeOfFile(char* name){
	FILE *f = fopen(name, "r");
	if (f == NULL)
		exit(1);
	int size;

	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);
	fclose(f);
	return size;
}

//from buffer to socket
void writeFromBuffer(int fd, int n, char* buffer, int toSend){
	int alreadySent = 0;
	while(toSend != alreadySent){
		n = write(fd, buffer, toSend - alreadySent);
		if(n == -1)
			exit(1);
		else
			alreadySent += n;
	}
}

//from file to socket
void writeFromFile(int fd, int n, char *filePath, int size){
	char *fileContent = (char *)malloc(BUFFERSIZE*sizeof(char));
	FILE *f = fopen(filePath, "r");
	if (f == NULL)
		exit(1);
	int alreadySent = 0;

	do{
		fseek(f, alreadySent, SEEK_SET);
		if (size < BUFFERSIZE){
			fread(fileContent, 1, size, f);
			n = write(fd, fileContent, size);
			if(n == -1)
				exit(1);
		} else {
			fread(fileContent, 1, BUFFERSIZE, f);
			n = write(fd, fileContent, BUFFERSIZE);
			if(n == -1)
				exit(1);
		}
		alreadySent += n;
		memset(fileContent, '\0', sizeof(char)*BUFFERSIZE);
	} while(alreadySent != size);

	fclose(f);
}

//send image to socket
void writeImageInfo(int newfd, int n, char *buffer, char *filePath, int size){
	char *iSize = (char *)malloc(10*sizeof(char));

	strcat(buffer, " 1 ");
	int len = strlen(filePath);
	for (int i=0; i<len; i++){
		if (strcmp(*(&filePath+i), ".") == 0){
			i++;
			strcat(buffer, *(&filePath+i));
			i++;
			strcat(buffer, *(&filePath+i));
			i++;
			strcat(buffer, *(&filePath+i));
		}
	}
	strcat(buffer, " ");
	size = sizeOfFile(filePath);
	snprintf(iSize, floor(log10 (size)) + 1, "%d", size);
	strcat(buffer, iSize);
	strcat(buffer, " ");

	int toSend = 8 + strlen(iSize);
	writeFromBuffer(newfd, n, buffer, toSend);
	writeFromFile(newfd, n, filePath, size);
}
/*
//from socket to buffer
void writeToBuffer(int fd, int n, char *buffer, int toReceive){
	int alreadyReceived = 0;
	while (alreadyReceived != toReceive){
		n = read(fd, buffer, toReceive - alreadyReceived);
		if(n == -1)
			exit(1);
		else
			alreadyReceived += n;
	}
}
*/
//from socket to buffer token by token
char *writeTokenToBuffer(int fd, int n, char *buffer){
	int end = 1;
	char *caracter = (char *)malloc(10*sizeof(char));
	int alreadyReceived = 0;
	while (end){
		printf("IN while\n");
		n = read(fd, caracter, 1);
		if(n == -1)
			printf("RIP\n");
			exit(1);
		buffer[alreadyReceived] = caracter[0];
		if ( (buffer[alreadyReceived] == ' ')/* || (buffer[alreadyReceived] == '\n') */){
			buffer[alreadyReceived] = '\0';
			end = 0;
		}
		alreadyReceived += n;
	}
	return buffer;
}

//from socket to file
void writeToFile(int fd, int n, char *filePath, int size){
	char *fileContent = (char *)malloc(BUFFERSIZE*sizeof(char));
	FILE *f = fopen(filePath, "w");
	if (f == NULL)
		exit(1);
	int alreadyReceived = 0;

	do{
		fseek(f, alreadyReceived, SEEK_SET);
		if (size < BUFFERSIZE){
			n = read(fd, fileContent, size);
		} else {
			n = read(fd, fileContent, BUFFERSIZE);
		}
		if(n == -1)
			exit(1);
		fwrite(fileContent, 1, n, f);
		alreadyReceived += n;
		memset(fileContent, '\0', sizeof(char)*BUFFERSIZE);
	} while(alreadyReceived != size);

	fclose(f);
}

void connectTCP(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr){
	n=connect(fd,res->ai_addr,res->ai_addrlen);
	if(n==-1)
		exit(1);
}

void question_get(int newfd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *parse, char *topic, char *question) {
	char *questionFilePath = (char *)malloc(100*sizeof(char));
	char *questionImageFilePath = (char *)malloc(100*sizeof(char));
	char *answerFilePath = (char *)malloc(100*sizeof(char));
	char *answerImageFilePath = (char *)malloc(100*sizeof(char));
	char *extension = (char *)malloc(5*sizeof(char));
	char *answerID = (char *)malloc(2*sizeof(char));

	//write the user request to server
	strcat(buffer, "GQU ");
	strcat(buffer, topic);
	strcat(buffer, " ");
	strcat(buffer, question);
	strcat(buffer, "\n");

	printf("%s\n", buffer);

	int toSend = strlen(buffer);
	while(toSend > 0){
		n = write(newfd, buffer, toSend);
		if(n == -1)
			exit(1);
		toSend -= n;
		buffer += n;
	}
	close(newfd);
	exit(0);
	
	//int toSend = 6 + strlen(topic) + strlen(question);
	writeFromBuffer(newfd, n, buffer, toSend);
	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

	strcat(questionFilePath, "/");
	strcat(questionFilePath, topic);
	strcat(questionFilePath, "/");
	strcat(questionFilePath, question);
	strcat(questionFilePath, ".txt");

	n = read(newfd, buffer, BUFFERSIZE);
	if(n == -1)
		exit(1);
	printf("%s\n", buffer);

	//read info from the server token by token
	char caracter[1];
	char qUserID[5];
	while(1) {
		n = read(newfd, caracter, 1);
		if(n == -1)
			exit(1);
		if((strcmp(caracter, "\n") == 0))
			break;
		strcat(qUserID, caracter);
	}
	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

	while(1) {
		n = read(newfd, caracter, 1);
		if(n == -1)
			exit(1);
		if((strcmp(caracter, "\n") == 0))
			break;
		strcat(buffer, caracter);
	}
	int qsize = atol(buffer);

	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

	//write to the question file
	writeToFile(newfd, n, questionFilePath, qsize);

	while(1) {
		n = read(newfd, caracter, 1);
		if(n == -1)
			exit(1);
		if((strcmp(caracter, "\n") == 0))
			break;
		strcat(buffer, caracter);
	}
	
	int qIMG = atol(buffer);

	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

	if (qIMG){

		while(1) {
			n = read(newfd, caracter, 1);
			if(n == -1)
				exit(1);
			if((strcmp(caracter, "\n") == 0))
				break;
			strcat(buffer, caracter);
		}
		strcpy(extension, buffer);
		memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

		strcat(questionImageFilePath, "/");
		strcat(questionImageFilePath, topic);
		strcat(questionImageFilePath, "/");
		strcat(questionImageFilePath, question);
		strcat(questionImageFilePath, extension);

		while(1) {
			n = read(newfd, caracter, 1);
			if(n == -1)
				exit(1);
			if((strcmp(caracter, "\n") == 0))
				break;
			strcat(buffer, caracter);
		}

		int qisize = atol(buffer);
		memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

		writeToFile(newfd, n, questionImageFilePath, qisize);
	}

	while(1) {
		n = read(newfd, caracter, 1);
		if(n == -1)
			exit(1);
		if((strcmp(caracter, "\n") == 0))
			break;
		strcat(buffer, caracter);
	}

	int n_ans = atol(buffer);
	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
	
	char* aUserID = (char *)malloc(5*sizeof(char));
	for (int i=0; i<n_ans; i++){
		while(1) {
			n = read(newfd, caracter, 1);
			if(n == -1)
				exit(1);
			if((strcmp(caracter, "\n") == 0))
				break;
			strcat(aUserID, caracter);
		}

		strcat(answerFilePath, "/");
		strcat(answerFilePath, topic);
		strcat(answerFilePath, "/");
		strcat(answerFilePath, question);
		strcat(answerFilePath, "_");
		strcat(answerFilePath, aUserID);
		strcat(answerFilePath, ".txt");
		memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
		memset(aUserID, '\0', sizeof(char)*5);

		while(1) {
			n = read(newfd, caracter, 1);
			if(n == -1)
				exit(1);
			if((strcmp(caracter, "\n") == 0))
				break;
			strcat(buffer, caracter);
		}
		int asize = atol(buffer);
		memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

		writeToFile(newfd, n, answerFilePath, asize);

		while(1) {
			n = read(newfd, caracter, 1);
			if(n == -1)
				exit(1);
			if((strcmp(caracter, "\n") == 0))
				break;
			strcat(buffer, caracter);
		}

		int aIMG = atol(buffer);
		memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
		if (aIMG){
			while(1) {
				n = read(newfd, caracter, 1);
				if(n == -1)
					exit(1);
				if((strcmp(caracter, "\n") == 0))
					break;
				strcat(buffer, caracter);
			}
			strcpy(extension, buffer);
			memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

			while(1) {
				n = read(newfd, caracter, 1);
				if(n == -1)
					exit(1);
				if((strcmp(caracter, "\n") == 0))
					break;
				strcat(buffer, caracter);
			}
			strcpy(extension, buffer);
			memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

			while(1) {
				n = read(newfd, caracter, 1);
				if(n == -1)
					exit(1);
				if((strcmp(caracter, "\n") == 0))
					break;
				strcat(buffer, caracter);
			}
			strcpy(answerID, buffer);
			memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

			strcat(answerImageFilePath, "/");
			strcat(answerImageFilePath, topic);
			strcat(answerImageFilePath, "/");
			strcat(answerImageFilePath, question);
			strcat(answerImageFilePath, "_");
			strcat(answerImageFilePath, answerID);
			strcat(answerImageFilePath, extension);
			memset(extension, '\0', sizeof(char)*10);
			memset(answerID, '\0', sizeof(char)*2);

			while(1) {
				n = read(newfd, caracter, 1);
				if(n == -1)
					exit(1);
				if((strcmp(caracter, "\n") == 0))
					break;
				strcat(buffer, caracter);
			}

			int aisize = atol(buffer);
			memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

			writeToFile(newfd, n, answerImageFilePath, aisize);
		}
		memset(answerFilePath, '\0', sizeof(char)*100);
		memset(answerImageFilePath, '\0', sizeof(char)*100);
	}

/*
	memset(questionFilePath, '\0', sizeof(char)*100);
	memset(questionImageFilePath, '\0', sizeof(char)*100);
	write(1, "stored files:\n", 14);
	write(1, questionFilePath, strlen(questionFilePath));

	write(1, "Q: ", 3);
	
	int qCaracters = atol(parse);
	char subString[qCaracters];
	memcpy(subString, &parse[11+strlen(parse)], qCaracters);
	write(1, subString, qCaracters);
*/
}

void question_submit(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *parse, char *userID, 
		char *topic, char *question, char *textFile, char *imageFile){
	char *qSize = (char *)malloc(10*sizeof(char));
	
	connectTCP(fd, addrlen, n, res, addr);

	//write the user request to server
	strcat(buffer, "QUS ");
	strcat(buffer, userID);
	strcat(buffer, " ");
	strcat(buffer, topic);
	strcat(buffer, " ");
	strcat(buffer, question);
	strcat(buffer, " ");
	int size = sizeOfFile(textFile);
	snprintf(qSize, floor(log10 (size)) + 1, "%d", size);
	strcat(buffer, qSize);
	strcat(buffer, " ");
	int toSend = 13 + strlen(topic) + strlen(question) + strlen(qSize);
	writeFromBuffer(fd, n, buffer, toSend);
	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
	writeFromFile(fd, n, textFile, size);
	if (imageFile == NULL){
		strcat(buffer, " 0");
		writeFromBuffer(fd, n, buffer, 2);
		memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
	} else {
		writeImageInfo(fd, n, buffer, imageFile, size);
	}
	strcat(buffer, "\n");
	writeFromBuffer(fd, n, buffer, 1);
	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
printf("send OK\n");
	
	//read info from the server token by token
	writeTokenToBuffer(fd, n, buffer);
	parse = writeTokenToBuffer(fd, n, buffer);
printf("return OK\n");
	
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
	int size;
	char *aSize = (char *)malloc(10*sizeof(char));

	connectTCP(fd, addrlen, n, res, addr);

	//write the user request to server
	strcat(buffer, "ANS ");
	strcat(buffer, userID);
	strcat(buffer, " ");
	strcat(buffer, topic);
	strcat(buffer, " ");
	strcat(buffer, question);
	strcat(buffer, " ");
	size = sizeOfFile(textFile);
	snprintf(aSize, floor(log10 (size)) + 1, "%d", size);
	strcat(buffer, aSize);
	strcat(buffer, " ");
	
	int toSend = 13 + strlen(topic) + strlen(question) + strlen(aSize);
	writeFromBuffer(fd, n, buffer, toSend);
	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

	writeFromFile(fd, n, textFile, size);

	if (imageFile == NULL){
		strcat(buffer, " 0");
		writeFromBuffer(fd, n, buffer, 2);
		memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
	} else {
		writeImageInfo(fd, n, buffer, imageFile, size);
	}
	strcat(buffer, "\n");
	writeFromBuffer(fd, n, buffer, 1);
	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

	//read info from the server token by token
	writeTokenToBuffer(fd, n, buffer);
	parse = writeTokenToBuffer(fd, n, buffer);

	if ((strcmp(parse, "OK") == 0)) {
		write(1, "Topic  was successfully created\n", 32);
	} else if ((strcmp(parse, "FUL") == 0)) {
		write(1, "Topic list is full\n", 19);
	} else {
		write(1, "Topic has not been created\n", 27);
	}
}
