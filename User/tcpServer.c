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
#include "tcpServer.h"
#include "data.h"
#define BUFFERSIZE 10000

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

void readTokenFromServer(int fd, int n, char *buffer){
	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
	char caracter[1];
	while(1) {
		n = read(fd, caracter, 1);
		printf("-%s-\n", caracter);
		if(n == -1)
			exit(1);
		if((strcmp(caracter, " ") == 0) || (strcmp(caracter, "\n") == 0))
			break;
		strcat(buffer, caracter);
	}
}

void writeTokenToServer(int fd, int n, char *buffer) {
	int toSend = strlen(buffer);
	char* ptr = buffer;
	while(toSend > 0) {
		n = write(fd, ptr, toSend);
		if(n == -1)
			exit(1);
		toSend -= n;
		ptr += n;
	}
	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
}

//from socket to file
void writeToFile(int fd, int n, char *buffer, char *filePath, int size){
	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
	FILE *f = fopen(filePath, "w");
	if (f == NULL)
		exit(1);

	int toSend = size;
	int alreadyReceived = 0;
	while(toSend > 0) {
		fseek(f, alreadyReceived, SEEK_SET);
		if (toSend < BUFFERSIZE){
			n = read(fd, buffer, toSend);
			printf("%s", buffer);
		} else {
			n = read(fd, buffer, BUFFERSIZE);
			printf("%s", buffer);
		}
		if(n == -1)
			exit(1);
		fwrite(buffer, 1, n, f);
		toSend -= n;
		alreadyReceived += n;
		memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
	}
	fclose(f);
}

void writeFromFile(int fd, int n, char *buffer, char *filePath, int size){
	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
	FILE *f = fopen(filePath, "r");
	size = sizeOfFile(filePath);
	if (f == NULL)
		exit(1);

	int toSend = size;
	int alreadyReceived = 0;
	while(toSend > 0) {
		fseek(f, alreadyReceived, SEEK_SET);
		if (toSend < BUFFERSIZE){
			n=toSend;
			fread(buffer, 1, toSend, f);
			printf("%s", buffer);
			//n = write(fd, buffer, toSend);
			writeTokenToServer(fd, n, buffer);
		} else {
			n=BUFFERSIZE;
			fread(buffer, 1, BUFFERSIZE, f);
			printf("%s", buffer);
			//n = write(fd, buffer, BUFFERSIZE);
			writeTokenToServer(fd, n, buffer);
		}
		if(n == -1)
			exit(1);
		toSend -= n;
		//printf("toSend ->%d\n", toSend);
		alreadyReceived += n;
		//printf("alreadyReceived ->%d\n", alreadyReceived);
		memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
	}
	fclose(f);
}

/*void question_get(int fd, int addrlen, int n, char *buffer, char *parse, int newfd){
	struct question *questionInfo;
//	char *questionPath = (char *)malloc(100*sizeof(char));
	char *questionFilePath = "q.txt";
	char *imageFilePath = "i.ext";
	char *answerFilePath = "a.txt";
	char *answerImageFilePath = "ai.ext";
	char *qSize = (char *)malloc(10*sizeof(char));
	char *aSize = (char *)malloc(10*sizeof(char));
	char *n_answers_string = (char *)malloc(10*sizeof(char));
	char *answerID = (char *)malloc(10*sizeof(char));

	char *topic = strtok(NULL, " ");
	char *question = strtok(NULL, "\n");

	strcat(questionPath, "/");
	strcat(questionPath, topic);
	strcat(questionPath, "/");
	strcat(questionPath, question);
	strcat(questionPath, "/");
	strcat(questionPath, questionFilePath);
	
	int num = atol(question);
	if (num == 0){
		questionInfo = getQuestion(NULL, topic, question, 0);
	} else {
		questionInfo = getQuestion(NULL, topic, NULL, num);
	}
	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

	strcat(buffer, "QGR ");

	if ("no topic or question"){
		strcat(buffer, "EOF");
	} else if ("erro"){
		strcat(buffer, "ERR");
	} else {
		strcat(buffer, questionInfo->author);
		strcat(buffer, " ");
		int size = sizeOfFile(questionFilePath);
		snprintf(qSize, floor(log10 (size)) + 1, "%d", size);
		strcat(buffer, qSize);
		strcat(buffer, " ");

		int toSend = 11 + strlen(qSize);
		writeFromBuffer(newfd, n, buffer, toSend);
		memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
		writeFromFile(newfd, n, questionFilePath, size);
		if (questionInfo->imageFilePath == NULL){
			strcat(buffer, " 0");

			writeFromBuffer(newfd, n, buffer, 2);
			memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
		} else {
			writeImageInfo(newfd, n, buffer, imageFilePath, size);
			memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
		}
		strcat(buffer, " ");
		snprintf(n_answers_string, floor(log10 (questionInfo->replies_number)) + 1, "%d", questionInfo->replies_number);
		strcat(buffer, n_answers_string);

		toSend = 1 + strlen(n_answers_string);
		writeFromBuffer(newfd, n, buffer, toSend);
		memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

		struct answer *answerInfo;
		for (int i=0; i<10 && i<questionInfo->replies_number; i++){
			answerInfo = questionInfo->answers;

			strcat(buffer, " ");
			snprintf(answerID, floor(log10 (answerInfo->number)) + 1, "%d", answerInfo->number);
			strcat(buffer, answerID);
			strcat(buffer, " ");
			strcat(buffer, answerInfo->author);
			strcat(buffer, " ");
			size = sizeOfFile(answerFilePath);
			snprintf(aSize, floor(log10 (size)) + 1, "%d", size);
			strcat(buffer, aSize);
			strcat(buffer, " ");

			toSend = 4 + strlen(answerID) + strlen(answerInfo->author) + strlen(aSize);
			writeFromBuffer(newfd, n, buffer, toSend);
			memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
			writeFromFile(newfd, n, answerFilePath, size);

			if (answerInfo->imageFilePath == NULL){
				strcat(buffer, " 0");

				writeFromBuffer(newfd, n, buffer, 2);
				memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
			} else {
				writeImageInfo(newfd, n, buffer, answerImageFilePath, size);
			}
			answerInfo = answerInfo->next;
		}
	//}
	strcat(buffer, "\n");
	writeFromBuffer(newfd, n, buffer, 1);
	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
}*/

void question_submit(int fd, int addrlen, int n, char *buffer, char *parse, int newfd){
	char *qUserID = (char *)malloc(5*sizeof(char));
	char *topic = (char *)malloc(10*sizeof(char));
	char *question = (char *)malloc(10*sizeof(char));
	char *qsize = (char *)malloc(10*sizeof(char));
	char *targetPath = (char *)malloc(100*sizeof(char));
	char *qIMG = (char *)malloc(1*sizeof(char));
	char *iext = (char *)malloc(3*sizeof(char));
	char *isize = (char *)malloc(100*sizeof(char));

	readTokenFromServer(newfd, n, buffer);

	readTokenFromServer(newfd, n, buffer);
	strcpy(qUserID, buffer);

	readTokenFromServer(newfd, n, buffer);
	strcpy(topic, buffer);

	readTokenFromServer(newfd, n, buffer);
	strcpy(question, buffer);

	readTokenFromServer(newfd, n, buffer);
	strcpy(qsize, buffer);
	int size = atol(qsize);

	saveNewQuestion(getTopic(topic), question, qUserID, NULL);
	generateFilePath(topic, question, NULL, NULL, targetPath);
	writeToFile(newfd, n, buffer, targetPath, size);
	
	memset(targetPath, '\0', sizeof(char)*100);

	readTokenFromServer(newfd, n, buffer);
	strcpy(qIMG, buffer);

	if(atol(qIMG) == 1) {
		readTokenFromServer(newfd, n, buffer);
		strcpy(iext, buffer);

		readTokenFromServer(newfd, n, buffer);
		strcpy(isize, buffer);
		size = atol(isize);

		generateFilePath(topic, question, NULL, iext, targetPath);
		saveNewQuestion(getTopic(topic), question, qUserID, targetPath);
		writeToFile(newfd, n, buffer, targetPath, size);
		memset(targetPath, '\0', sizeof(char)*100);
	}
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
	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

	int toSend = 5;
	strcat(buffer, "ANR ");
	/*if ("erro"){
		strcat(buffer, "ERR");
		toSend += 3;
	} else if ("answers list full"){
		strcat(buffer, "FUL");
		toSend += 3;
	} else if ("erro"){
		strcat(buffer, "NOK");
		toSend += 3;
	} else {*/
		strcat(buffer, "OK");
		toSend += 2;
	//}
	strcat(buffer, "\n");
	//writeFromBuffer(newfd, n, buffer, toSend);
	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
}

