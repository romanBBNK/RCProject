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

int sizeOfFile2(char* name){
	FILE *f = fopen(name, "rb");
	if (f == NULL)
		exit(1);
	int size;

	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);
	fclose(f);
	return size;
}

//from socket to buffer token by token
void readTokenFromServer(int fd, int n, char *buffer){
	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
	char caracter[1];
	while(1) {
		n = read(fd, caracter, 1);
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

void writeTokenToServer2(int fd, int n, char *buffer, int size) {
	int toSend = size;
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
			//n = write(fd, buffer, toSend);
			writeTokenToServer(fd, n, buffer);
		} else {
			n=BUFFERSIZE;
			fread(buffer, 1, BUFFERSIZE, f);
			//n = write(fd, buffer, BUFFERSIZE);
			writeTokenToServer(fd, n, buffer);
		}
		if(n == -1)
			exit(1);
		toSend -= n;
		alreadyReceived += n;
		memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
	}
	fclose(f);
}

void writeFromFile2(int fd, int n, char *buffer, char *filePath, int size){
	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
	FILE *f = fopen(filePath, "rb");
	size = sizeOfFile2(filePath);
	if (f == NULL)
		exit(1);

	int toSend = size;
	int alreadyReceived = 0;
	while(toSend > 0) {
		fseek(f, alreadyReceived, SEEK_SET);
		if (toSend < BUFFERSIZE){
			n=toSend;
			fread(buffer, 1, toSend, f);
			//n = write(fd, buffer, toSend);
			writeTokenToServer(fd, n, buffer);
		} else {
			n=BUFFERSIZE;
			fread(buffer, 1, BUFFERSIZE, f);
			//n = write(fd, buffer, BUFFERSIZE);
			writeTokenToServer(fd, n, buffer);
		}
		if(n == -1)
			exit(1);
		toSend -= n;
		alreadyReceived += n;
		memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
	}
	fclose(f);
}

void question_get(int fd, int addrlen, int n, char *buffer, char *parse, int newfd){
	struct question *questionInfo;
//	char *questionPath = (char *)malloc(100*sizeof(char));
	char *topic = (char *)malloc(10*sizeof(char));
	char *question = (char *)malloc(10*sizeof(char));
	char *qSize = (char *)malloc(10*sizeof(char));
	char *iSize = (char *)malloc(10*sizeof(char));
	char *aSize = (char *)malloc(10*sizeof(char));
	char *aiSize = (char *)malloc(10*sizeof(char));
	char *n_answers_string = (char *)malloc(10*sizeof(char));
	char *answerID = (char *)malloc(10*sizeof(char));
	char *extension = (char *)malloc(5*sizeof(char));
	char *questionFilePath = (char *)malloc(64*sizeof(char));
	char *imageFilePath = (char *)malloc(64*sizeof(char));
	char *answerFilePath = (char *)malloc(64*sizeof(char));
	char *answerImageFilePath = (char *)malloc(64*sizeof(char));

	//topic
	readTokenFromServer(newfd, n, buffer);
	strcpy(topic, buffer);
	printf("TOPIC:%s\n", topic);

	//question
	readTokenFromServer(newfd, n, buffer);
	strcpy(question, buffer);
	printf("question:%s\n", question);

	generateFilePath(topic, question, NULL, NULL, questionFilePath);
	printf("%s\n", questionFilePath);
	int num = atol(question);
	if (num == 0){
		questionInfo = getQuestion(topic, question, 0);
	} else {
		questionInfo = getQuestion(topic, NULL, num);
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
		sprintf(qSize, "%d", size);
		strcat(buffer, qSize);
		strcat(buffer, " ");
		writeTokenToServer(newfd, n, buffer);
		writeFromFile(newfd, n, buffer, questionFilePath, size);
		
		printf("IMAGE EXT:%s\n", questionInfo->imgExt);
//////////////////////
/*		memset(imageFilePath, '\0', 64*sizeof(char));
		strcat(imageFilePath, "./Data/");
		strcat(imageFilePath, topic);
		strcat(imageFilePath, "/");
		strcat(imageFilePath, question);		
		strcat(imageFilePath, "/");
		strcat(imageFilePath, question);
		//strcat(imageFilePath, questionInfo->imgExt);
		strcat(imageFilePath, ".jpg");
*//////////////////////////
		if (1/*imageFilePath == NULL*/){//TODO
			printf("NO IMAGE\n");
			strcat(buffer, " 0");

			writeTokenToServer(newfd, n, buffer);
		} else {
			printf("YES\n");
			generateFilePath(topic, question, NULL, questionInfo->imgExt, imageFilePath);
			printf("path -%s\n", imageFilePath);

			strcat(buffer, " 1 ");
			strcat(buffer, questionInfo->imgExt);
			printf("IMAGE PATH:%s\n", imageFilePath);
			strcat(buffer, " ");
			size = sizeOfFile2(imageFilePath);
			sprintf(iSize, "%d", size);
			strcat(buffer, iSize);
			strcat(buffer, " ");
			writeTokenToServer(newfd, n, buffer);
			writeFromFile2(newfd, n, buffer, imageFilePath, size);
			printf("end HAS IMAGE\n");
		}
		strcat(buffer, " ");
		sprintf(n_answers_string, "%d", questionInfo->replies_number);
		strcat(buffer, n_answers_string);

		writeTokenToServer(newfd, n, buffer);

		struct answer *answerInfo;
		for (int i=0; i<10 && i<questionInfo->replies_number; i++){
			answerInfo = questionInfo->answers;
			generateFilePath(topic, question, answerInfo->name, NULL, answerFilePath);

			strcat(buffer, " ");
			sprintf(answerID, "%d", answerInfo->number);
			strcat(buffer, answerID);
			strcat(buffer, " ");
			strcat(buffer, answerInfo->author);
			strcat(buffer, " ");
			size = sizeOfFile(answerFilePath);
			sprintf(aSize, "%d", size);
			strcat(buffer, aSize);
			strcat(buffer, " ");

			writeTokenToServer(newfd, n, buffer);
			writeFromFile(newfd, n, buffer, answerFilePath, size);

			if (1/*answerInfo-> == NULL*/){//TODO
				strcat(buffer, " 0");

				writeTokenToServer(newfd, n, buffer);
			} else {
				generateFilePath(topic, question, answerInfo->name, answerInfo->imgExt, answerImageFilePath);

				strcat(buffer, " 1 ");
				strcat(buffer, answerInfo->imgExt);
				strcat(buffer, " ");
				size = sizeOfFile(answerImageFilePath);
				sprintf(aiSize, "%d", size);
				strcat(buffer, aiSize);
				strcat(buffer, " ");
				writeTokenToServer(newfd, n, buffer);
				writeFromFile(newfd, n, buffer, answerImageFilePath, size);
			}
			answerInfo = answerInfo->next;
		}
	//}
	strcat(buffer, "\n");
	writeTokenToServer(newfd, n, buffer);
	printf("end\n");
}

void question_submit(int fd, int addrlen, int n, char *buffer, char *parse, int newfd){
	char *qUserID = (char *)malloc(5*sizeof(char));
	char *topic = (char *)malloc(10*sizeof(char));
	char *question = (char *)malloc(10*sizeof(char));
	char *qsize = (char *)malloc(10*sizeof(char));
	char *imageFilePath = (char *)malloc(100*sizeof(char));
	
	//qUserID
	readTokenFromServer(newfd, n, buffer);
	strcpy(qUserID, buffer);
	//topic
	readTokenFromServer(newfd, n, buffer);
	strcpy(topic, buffer);
	//question
	readTokenFromServer(newfd, n, buffer);
	strcpy(question, buffer);
	//qsize
	readTokenFromServer(newfd, n, buffer);
	strcpy(topic, buffer);

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
	//writeFromBuffer(newfd, n, buffer, toSend);
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
}

