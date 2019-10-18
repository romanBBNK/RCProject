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

void readTokenFromServer(int fd, int n, char *buffer){
	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
	char caracter[2];
	while(1) {
		n = read(fd, caracter, 1);
		if(n == -1)
			exit(1);
		printf("-%s-\n", caracter);
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
		printf("%s", ptr);
		toSend -= n;
		ptr += n;
	}
	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
}

void writeToFile(int fd, int n, char *buffer, char *filePath, int size){
	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
	FILE *f = fopen(filePath, "w");
	if (f == NULL)
		exit(1);

	int toSend = size;
	int alreadyReceived = 0;
	while(toSend > 0) {
		fseek(f, alreadyReceived, SEEK_SET);
		if (toSend < BUFFERSIZE-1){
			n = read(fd, buffer, toSend);
		} else {
			n = read(fd, buffer, BUFFERSIZE-1);
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

void writeToFile2(int fd, int n, char *buffer, char *filePath, int size){
	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
	FILE *f = fopen(filePath, "wb");
	if (f == NULL)
		exit(1);

	int toSend = size;
	int alreadyReceived = 0;
	while(toSend > 0) {
		fseek(f, alreadyReceived, SEEK_SET);
		if (toSend < BUFFERSIZE-1){
			n = read(fd, buffer, toSend);
		} else {
			n = read(fd, buffer, BUFFERSIZE-1);
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
		if (toSend < BUFFERSIZE-1){
			n=toSend;
			fread(buffer, 1, toSend, f);
			//printf("\nREADbuffer->//INICIO// %s //FIM//\n",buffer);
			//n = write(fd, buffer, toSend);
			writeTokenToServer(fd, n, buffer);
		} else {
			n=BUFFERSIZE-1;
			fread(buffer, 1, BUFFERSIZE-1, f);
			//printf("\nREADbuffer->//INICIO// %s //FIM//\n",buffer);
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
			//printf("\nREADbuffer->//INICIO// %s //FIM//\n",buffer);
			//n = write(fd, buffer, toSend);
			writeTokenToServer2(fd, n, buffer, toSend);
		} else {
			n=BUFFERSIZE;
			fread(buffer, 1, BUFFERSIZE, f);
			//printf("\nREADbuffer->//INICIO// %s //FIM//\n",buffer);
			//n = write(fd, buffer, BUFFERSIZE);
			writeTokenToServer2(fd, n, buffer, BUFFERSIZE);
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
	//char *questionPath = (char *)malloc(100*sizeof(char));
	char *topic = (char *)malloc(11*sizeof(char));
	char *question = (char *)malloc(11*sizeof(char));
	char *qSize = (char *)malloc(11*sizeof(char));
	char *iSize = (char *)malloc(11*sizeof(char));
	char *aSize = (char *)malloc(11*sizeof(char));
	char *aiSize = (char *)malloc(11*sizeof(char));
	char *n_answers_string = (char *)malloc(11*sizeof(char));
	char *answerID = (char *)malloc(11*sizeof(char));
	char *extension = (char *)malloc(6*sizeof(char));
	char *questionFilePath = (char *)malloc(65*sizeof(char));
	char *imageFilePath = (char *)malloc(65*sizeof(char));
	char *answerFilePath = (char *)malloc(65*sizeof(char));
	char *answerImageFilePath = (char *)malloc(65*sizeof(char));

	//topic
	readTokenFromServer(newfd, n, buffer);
	strcpy(topic, buffer);
	printf("TOPIC:%s\n", topic);

	//question
	readTokenFromServer(newfd, n, buffer);
	strcpy(question, buffer);
	printf("question:%s\n", question);

	generateFilePath(topic, question, NULL, NULL, questionFilePath);
	printf("questionPath:%s\n", questionFilePath);
	
	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
	strcat(buffer, "QGR ");

	questionInfo = getQuestion(topic, question, 0);

	strcat(buffer, questionInfo->author);
	printf("buffer:%s\n", buffer);
	strcat(buffer, " ");
	printf("buffer:%s\n", buffer);
	int size = sizeOfFile(questionFilePath);
	printf("size:%d\n", size);
	sprintf(qSize, "%d", size);
	printf("qsize:%s\n", qSize);
	strcat(buffer, qSize);
	printf("buffer:%s\n", buffer);
	strcat(buffer, " ");
	printf("buffer:%s\n", buffer);
	writeTokenToServer(newfd, n, buffer);
	printf("escreveu buffer\n");
	writeFromFile(newfd, n, buffer, questionFilePath, size);
	printf("%s\n", "oi");
	
	printf("IMAGE EXT:%s\n", questionInfo->imgExt);
	//////////////////////
	/*memset(imageFilePath, '\0', 64*sizeof(char));
	strcat(imageFilePath, "./Data/");
	strcat(imageFilePath, topic);
	strcat(imageFilePath, "/");
	strcat(imageFilePath, question);		
	strcat(imageFilePath, "/");
	strcat(imageFilePath, question);
	//strcat(imageFilePath, questionInfo->imgExt);
	strcat(imageFilePath, ".jpg");
	*//////////////////////////
		if ((strcmp(questionInfo->imgExt, "NULL")) == 0){//TODO
			printf("NO IMAGE\n");
			strcat(buffer, " 0");

			writeTokenToServer(newfd, n, buffer);
		} else {
			printf("YES\n");

			strcat(buffer, " 1 ");
			strcat(buffer, questionInfo->imgExt);
			generateFilePath(topic, question, NULL, questionInfo->imgExt, imageFilePath);
			printf("IMAGE PATH:%s\n", imageFilePath);
			strcat(buffer, " ");
			size = sizeOfFile2(imageFilePath);
			sprintf(iSize, "%d", size);
			printf("size:%d\n", size);
			strcat(buffer, iSize);
			strcat(buffer, " ");
			printf("buffer:%s\n", buffer);
			writeTokenToServer(newfd, n, buffer);
			writeFromFile2(newfd, n, buffer, imageFilePath, size);
			printf("end HAS IMAGE\n");
		}
		strcat(buffer, " ");
		sprintf(n_answers_string, "%d", questionInfo->replies_number);
		strcat(buffer, n_answers_string);

		printf("n_answers_string:%s\n", buffer);
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

			if ((strcmp(answerInfo->imgExt, "NULL")) == 0){//TODO
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
	char *qUserID = (char *)malloc(6*sizeof(char));
	char *topic = (char *)malloc(11*sizeof(char));
	char *question = (char *)malloc(11*sizeof(char));
	char *qsize = (char *)malloc(11*sizeof(char));
	char *targetPath = (char *)malloc(101*sizeof(char));
	char *qIMG = (char *)malloc(2*sizeof(char));
	char *iext = (char *)malloc(4*sizeof(char));
	char *isize = (char *)malloc(101*sizeof(char));
	int status;

	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
	memset(qUserID, '\0', sizeof(char)*6);
	memset(targetPath, '\0', sizeof(char)*101);

	//qUserID
	readTokenFromServer(newfd, n, buffer);
	strcpy(qUserID, buffer);
	printf("qUserID->%s\n", qUserID);

	//topic
	readTokenFromServer(newfd, n, buffer);
	strcpy(topic, buffer);
	printf("topic->%s\n", topic);

	//question
	readTokenFromServer(newfd, n, buffer);
	strcpy(question, buffer);
	printf("question->%s\n", question);

	//qsize
	readTokenFromServer(newfd, n, buffer);
	strcpy(qsize, buffer);
	int size = atol(qsize);
	printf("qSize->%s\n", qsize);

	//qdata
	generateFilePath(topic, question, NULL, NULL, targetPath);
	printf("   -targetPath->%s\n", targetPath);
	writeToFile(newfd, n, buffer, targetPath, size);
	printf("escrever data\n");

	memset(targetPath, '\0', sizeof(char)*101);

	//le " "
	readTokenFromServer(newfd, n, buffer);

	//qIMG
	readTokenFromServer(newfd, n, buffer);
	strcpy(qIMG, buffer);
	printf("qIMG->%s\n", qIMG);

	if(atol(qIMG) == 1) {
		//iext
		readTokenFromServer(newfd, n, buffer);
		strcpy(iext, buffer);
		printf("iext->%s\n", iext);

		////
		status = saveNewQuestion(getTopic(topic), question, qUserID, iext);
		//int status = 0;
		printf("   -status->%d\n", status);
		//strcpy(targetPath, "./question.txt");
		////

		//isize
		readTokenFromServer(newfd, n, buffer);
		strcpy(isize, buffer);
		size = atol(isize);
		printf("iSize->%s\n", isize);

		//idata
		strcpy(getQuestion(topic, question, 0)->imgExt, iext);
		generateFilePath(topic, question, NULL, iext, targetPath);

		//strcpy(targetPath, "./imagem.jpg");
		writeToFile2(newfd, n, buffer, targetPath, size);
		memset(targetPath, '\0', sizeof(char)*101);
		printf("escrever imagem\n");
	} else {
		////
		status = saveNewQuestion(getTopic(topic), question, qUserID, NULL);
		//int status = 0;
		printf("   -status->%d\n", status);
		//strcpy(targetPath, "./question.txt");
		////
	}

	if (status == 0) {
		write(1, "Question ", 9);
		write(1, question, strlen(question));
		write(1, " was successfully created by ", 29);
		write(1, qUserID, 5);
		write(1, "\n", 1);
		memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
		strcat(buffer, "QUR OK\n");
		writeTokenToServer(newfd, n, buffer);
		//writeTokenToServer(newfd, n, "QUR OK\n");
		printf("\noi1\n");
	} else if (status == 2) {
		write(1, "Question ", 9);
		write(1, question, strlen(question));
		write(1, " already exists\n", 16);
		memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
		strcat(buffer, "QUR DUP\n");
		writeTokenToServer(newfd, n, buffer);
		//writeTokenToServer(newfd, n, "QUR DUP\n");
	} else if (status == 1) {
		write(1, "Question list is full\n", 22);
		memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
		strcat(buffer, "QUR FUL\n");
		writeTokenToServer(newfd, n, buffer);
		//writeTokenToServer(newfd, n, "QUR FUL\n");
	} else {
		write(1, "Question ", 9);
		write(1, question, strlen(question));
		write(1, " has not been created\n", 22);
		memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
		strcat(buffer, "QUR NOK\n");
		writeTokenToServer(newfd, n, buffer);
		//writeTokenToServer(newfd, n, "QUR NOK\n");
	}

	free(qUserID);
	free(topic);
	free(question);
	free(qsize);
	free(targetPath);
	free(qIMG);
	free(iext);
	free(isize);
}

void answer_submit(int fd, int addrlen, int n, char *buffer, char *parse, int newfd){
	char *aUserID = (char *)malloc(6*sizeof(char));
	char *topic = (char *)malloc(11*sizeof(char));
	char *question = (char *)malloc(11*sizeof(char));
	char *asize = (char *)malloc(11*sizeof(char));
	char *targetPath = (char *)malloc(101*sizeof(char));
	char *aIMG = (char *)malloc(2*sizeof(char));
	char *iext = (char *)malloc(4*sizeof(char));
	char *isize = (char *)malloc(101*sizeof(char));
	char *answerName = (char *)malloc(14*sizeof(char));
	char *answerID = (char *)malloc(3*sizeof(char));
	int status;

	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
	memset(targetPath, '\0', sizeof(char)*101);

	//aUserID
	readTokenFromServer(newfd, n, buffer);
	strcpy(aUserID, buffer);
	printf("aUserID->%s\n", aUserID);

	//topic
	readTokenFromServer(newfd, n, buffer);
	strcpy(topic, buffer);
	printf("topic->%s\n", topic);

	//question
	readTokenFromServer(newfd, n, buffer);
	strcpy(question, buffer);
	printf("question->%s\n", question);

	//asize
	readTokenFromServer(newfd, n, buffer);
	strcpy(asize, buffer);
	int size = atol(asize);
	printf("asize->%s\n", asize);

	retrieveStoredData();

	int aID = getLastAnswerNumber(getQuestion(topic, question, 0));
	if(aID == -1) {
		aID = 1;
	} else {
		aID += 1;
	}
	sprintf(answerID, "%d", aID);
	strcpy(answerName, question);
	if(aID < 10){
		strcat(answerName, "_0");
		strcat(answerName, answerID);
	} else {
		strcat(answerName, "_");
		strcat(answerName, answerID);
	}
	printf(" -topic:%s\n", topic);
	printf(" -question:%s\n", question);
	printf(" -answerName:%s\n", answerName);
	generateFilePath(topic, question, answerName, NULL, targetPath);
	printf("filePath:%s\n", targetPath);
	printf("   -targetPath->%s\n", targetPath);
	writeToFile(newfd, n, buffer, targetPath, size);
	printf("escrever data\n");

	memset(targetPath, '\0', sizeof(char)*101);

	//le " "
	readTokenFromServer(newfd, n, buffer);

	//aIMG
	readTokenFromServer(newfd, n, buffer);
	strcpy(aIMG, buffer);
	printf("aIMG->%s\n", aIMG);

	if(atol(aIMG) == 1) {
		//iext
		readTokenFromServer(newfd, n, buffer);
		strcpy(iext, buffer);
		printf("iext->%s\n", iext);

		status = saveNewAnswer(topic, getQuestion(topic, question, 0), aUserID, iext);
		printf("   -status->%d\n", status);

		//isize
		readTokenFromServer(newfd, n, buffer);
		strcpy(isize, buffer);
		size = atol(isize);
		printf("iSize->%s\n", isize);

		//idata
		strcpy(getAnswer(topic, question, aID)->imgExt, iext);
		generateFilePath(topic, question, answerName, iext, targetPath);
		printf("filePath:%s\n", targetPath);

		//strcpy(targetPath, "./imagem.jpg");
		writeToFile2(newfd, n, buffer, targetPath, size);
		memset(targetPath, '\0', sizeof(char)*101);
		printf("escrever imagem\n");
	} else {
		int status = saveNewAnswer(topic, getQuestion(topic, question, 0), aUserID, NULL);
		printf("   -status->%d\n", status);
	}

	if (status == 0) {
		write(1, "Answer ", 7);
		write(1, parse, strlen(parse));
		write(1, " was successfully created by ", 29);
		write(1, aUserID, 5);
		write(1, "\n", 1);
		memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
		strcat(buffer, "ANR OK\n");
		writeTokenToServer(newfd, n, buffer);
		//writeTokenToServer(newfd, n, "QUR OK\n");
	} else if (status == 2) {
		write(1, "Answer ", 7);
		write(1, parse, strlen(parse));
		write(1, " already exists\n", 16);
		memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
		strcat(buffer, "ANR DUP\n");
		writeTokenToServer(newfd, n, buffer);
		//writeTokenToServer(newfd, n, "QUR DUP\n");
	} else if (status == 1) {
		write(1, "Answer list is full\n", 19);
		memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
		strcat(buffer, "ANR FUL\n");
		writeTokenToServer(newfd, n, buffer);
		//writeTokenToServer(newfd, n, "ANR FUL\n");
	} else {
		write(1, "Answer ", 7);
		write(1, parse, strlen(parse));
		write(1, " has not been created\n", 22);
		memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
		strcat(buffer, "ANR NOK\n");
		writeTokenToServer(newfd, n, buffer);
		//writeTokenToServer(newfd, n, "QUR NOK\n");
	}

	free(aUserID);
	free(topic);
	free(question);
	free(asize);
	free(targetPath);
	free(aIMG);
	free(iext);
	free(isize);
	free(answerName);
	free(answerID);
}