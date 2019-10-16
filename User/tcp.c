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
	//writeFromBuffer(newfd, n, buffer, toSend);
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

//from socket to file
void writeToFile(int fd, int n, char *filePath, int size){
	/*char *fileContent = (char *)malloc(BUFFERSIZE*sizeof(char));
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

	fclose(f);*/
}


/*
void writeToFile(int fd, int n, char *topic, char *question, char *ext, int size, char* buffer) {
	//abrir ficheiro etc
	//meter na pasta do lado do cliente
	char* ptr = buffer;
	int toReceive = size;
	while(toReceive > 0){
		n = read(newfd, ptr, toReceive);
		if(n == -1)
			exit(1);
		//escreve no fiheiro
		toReceive -= n;
		ptr += n;
	}
}

void readFromFile(int fd, int n, char *filePath, char* buffer) {
	//abrir ficheiro etc

	char size = //size da imagem/texto - tem de ir buscar ao ficheiro
	char ext = //extensao - tem de ir buscar ao ficheiro
	strcat(buffer, size);
	strcat(buffer, " ");
	strcat(buffer, ext);
	strcat(buffer, " ");

	int size = strlen(buffer);
	while(size > 0){
		n = write(newfd, ptr, size);
		if(n == -1)
			exit(1);
		size -= n;
		ptr += n;
	}

	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

	char* ptr;
	int toSend = atol(size);
	int eachBuffer;
	while(toSend > 0){
		//vai buscar ao ficheiro
		//mete no buffer - enche só até à capacidade dele (BUFFERSIZE)
		eachBuffer = strlen(buffer);
		ptr = buffer;
		while(eachBuffer > 0) {
			n = write(newfd, ptr, eachBuffer);
			if(n == -1)
				exit(1);
			eachBuffer -= n;
			toSend -= n;
			ptr += n;
		}
		memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
	}
}*/

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

	char* ptr = buffer;
	int toSend = strlen(buffer);
	while(toSend > 0){
		n = write(newfd, ptr, toSend);
		if(n == -1)
			exit(1);
		toSend -= n;
		ptr += n;
	}

	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

	strcat(questionFilePath, "/");
	strcat(questionFilePath, topic);
	strcat(questionFilePath, "/");
	strcat(questionFilePath, question);
	strcat(questionFilePath, ".txt");

	//read info from the server token by token
	char caracter[1];
	char *qUserID = (char *)malloc(5*sizeof(char));
	while(1) {
		n = read(newfd, caracter, 1);
		if(n == -1)
			exit(1);
		if((strcmp(caracter, " ") == 0))
			break;
	}

	//qUserID
	while(1) {
		n = read(newfd, caracter, 1);
		if(n == -1)
			exit(1);
		if((strcmp(caracter, " ") == 0))
			break;
		strcat(qUserID, caracter);
	}

	printf("qUserID -> %s\n", qUserID);

	//qsize
	readTokenFromServer(newfd, n, buffer);


	int qsize = atol(buffer);

	printf("qsize -> %d\n", qsize);

	//write to the question file
	//writeToFile(newfd, n, questionFilePath, qsize);
	readTokenFromServer(newfd, n, buffer);

	//qIMG
	readTokenFromServer(newfd, n, buffer);

	int qIMG = atol(buffer);

	printf("qIMG -> %d\n", qIMG);

	if (qIMG){
		//qiext
		readTokenFromServer(newfd, n, buffer);

		strcpy(extension, buffer);
		
		printf("qiext -> %s\n", extension);

		strcat(questionImageFilePath, "/");
		strcat(questionImageFilePath, topic);
		strcat(questionImageFilePath, "/");
		strcat(questionImageFilePath, question);
		strcat(questionImageFilePath, extension);

		//isize
		readTokenFromServer(newfd, n, buffer);

		int qisize = atol(buffer);

		printf("qisize -> %d\n", qisize);

		//writeToFile(newfd, n, questionImageFilePath, qisize);
		readTokenFromServer(newfd, n, buffer);
	}

	//N
	readTokenFromServer(newfd, n, buffer);

	int n_ans = atol(buffer);

	printf("N -> %d\n", n_ans);
	
	char* aUserID = (char *)malloc(5*sizeof(char));
	for (int i=0; i<n_ans; i++){
		//aUserID
		readTokenFromServer(newfd, n, buffer);

		printf("aUserID -> %s\n", aUserID);

		strcat(answerFilePath, "/");
		strcat(answerFilePath, topic);
		strcat(answerFilePath, "/");
		strcat(answerFilePath, question);
		strcat(answerFilePath, "_");
		strcat(answerFilePath, aUserID);
		strcat(answerFilePath, ".txt");
		memset(aUserID, '\0', sizeof(char)*5);

		//asize
		readTokenFromServer(newfd, n, buffer);

		int asize = atol(buffer);

		printf("asize -> %d\n", asize);

		//writeToFile(newfd, n, answerFilePath, asize);
		readTokenFromServer(newfd, n, buffer);

		readTokenFromServer(newfd, n, buffer);

		int aIMG = atol(buffer);
		
		printf("aIMG -> %d\n", aIMG);
		if (aIMG){
			//aiext
			readTokenFromServer(newfd, n, buffer);

			strcpy(extension, buffer);

			printf("aiext -> %s\n", extension);

			//AN
			readTokenFromServer(newfd, n, buffer);
			strcpy(answerID, buffer);
			
			printf("AN -> %s\n", answerID);

			strcat(answerImageFilePath, "/");
			strcat(answerImageFilePath, topic);
			strcat(answerImageFilePath, "/");
			strcat(answerImageFilePath, question);
			strcat(answerImageFilePath, "_");
			strcat(answerImageFilePath, answerID);
			strcat(answerImageFilePath, extension);
			memset(extension, '\0', sizeof(char)*10);
			memset(answerID, '\0', sizeof(char)*2);

			//aisize
			readTokenFromServer(newfd, n, buffer);

			int aisize = atol(buffer);

			printf("aisize -> %d\n", aisize);

			//writeToFile(newfd, n, answerImageFilePath, aisize);
			readTokenFromServer(newfd, n, buffer);
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

	writeTokenToServer(fd, n, buffer);
	// !!!!! writeToFile(fd, n, buffer);

	//writeFromBuffer(fd, n, buffer, toSend);

	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
	writeFromFile(fd, n, textFile, size);
	if (imageFile == NULL){
		strcat(buffer, " 0");
		writeTokenToServer(fd, n, buffer);
	} else {
		writeImageInfo(fd, n, buffer, imageFile, size);
	}
	strcat(buffer, "\n");
	//writeFromBuffer(fd, n, buffer, 1);
	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
	printf("send OK\n");
	
	/*
	//read info from the server token by token
	readTokenFromServer(fd, n, buffer);
	parse = readTokenFromServer(fd, n, buffer);
	printf("return OK\n");
	
	if ((strcmp(parse, "OK") == 0)) {
		write(1, "Topic  was successfully created\n", 32);
	} else if ((strcmp(parse, "DUP") == 0)) {
		write(1, "Topic  already exists\n", 22);
	} else if ((strcmp(parse, "FUL") == 0)) {
		write(1, "Topic list is full\n", 19);
	} else {
		write(1, "Topic has not been created\n", 27);
	}*/
}

void answer_submit(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *parse, char *userID, 
		char *topic, char *question, char *textFile, char *imageFile){
	int size;
	char *aSize = (char *)malloc(10*sizeof(char));

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
	//writeFromBuffer(fd, n, buffer, toSend);
	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

	writeFromFile(fd, n, textFile, size);

	if (imageFile == NULL){
		strcat(buffer, " 0");
		//writeFromBuffer(fd, n, buffer, 2);
		memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
	} else {
		writeImageInfo(fd, n, buffer, imageFile, size);
	}
	strcat(buffer, "\n");
	//writeFromBuffer(fd, n, buffer, 1);
	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

	//read info from the server token by token
	readTokenFromServer(fd, n, buffer);
	/*parse = */readTokenFromServer(fd, n, buffer);

	if ((strcmp(parse, "OK") == 0)) {
		write(1, "Topic  was successfully created\n", 32);
	} else if ((strcmp(parse, "FUL") == 0)) {
		write(1, "Topic list is full\n", 19);
	} else {
		write(1, "Topic has not been created\n", 27);
	}
}
