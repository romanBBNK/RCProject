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
	while(toSend > 0){
		n = write(fd, buffer, toSend);
		if(n == -1)
			exit(1);
		toSend -= n;
		buffer += n;
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

void question_get(int fd, int addrlen, int n, char *buffer, char *parse, int newfd){
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
/*
	strcat(questionPath, "/");
	strcat(questionPath, topic);
	strcat(questionPath, "/");
	strcat(questionPath, question);
	strcat(questionPath, "/");
	strcat(questionPath, questionFilePath);
*/	
	int num = atol(question);
	if (num == 0){
		questionInfo = getQuestion(NULL, topic, question, 0);
	} else {
		questionInfo = getQuestion(NULL, topic, NULL, num);
	}
	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

	strcat(buffer, "QGR ");

	/*if ("no topic or question"){
		strcat(buffer, "EOF");
	} else if ("erro"){
		strcat(buffer, "ERR");
	} else {*/
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
}

void question_submit(int fd, int addrlen, int n, char *buffer, char *parse, int newfd){
	char *qUserID = (char *)malloc(5*sizeof(char));
	char *topic = (char *)malloc(10*sizeof(char));
	char *question = (char *)malloc(10*sizeof(char));
	char *qsize = (char *)malloc(10*sizeof(char));
	char *imageFilePath = (char *)malloc(100*sizeof(char));
	

	qUserID = strtok(NULL, " ");
	topic = strtok(NULL, " ");
	question = strtok(NULL, " "); // question / question_number
	qsize = strtok(NULL, " ");

	//int size = atol(qsize);
	/*char *qdata = (char *)malloc(size*sizeof(char));
	for (int i=0; i<size; i++){
		char c = getchar();
		qdata[i] = c;
	}
	getchar();
  */parse = strtok(NULL, " "); // parse = qIMG
	if (strcmp(parse,"1") == 0){
		parse = strtok(NULL, " "); // parse = iext
		parse = strtok(NULL, " "); // parse = isize
		parse = strtok(NULL, " "); // parse = idata
	}
	parse = strtok(NULL, "\n");

	addNewQuestion(getTopic(topic), question, qUserID, imageFilePath);

	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

	int toSend = 5;
	strcat(buffer, "QUR ");
	/*if ("question already exist"){
		strcat(buffer, "DUP");
		toSend += 3;
	} else if ("questions list full"){
		strcat(buffer, "FUL");
		toSend += 3;
	} else if ("erro"){
		strcat(buffer, "NOK");
		toSend += 3;
	} else {
	*/	strcat(buffer, "OK");
		toSend += 2;
	//}
	strcat(buffer, "\n");
	writeFromBuffer(newfd, n, buffer, toSend);
	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
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
	writeFromBuffer(newfd, n, buffer, toSend);
	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
}

