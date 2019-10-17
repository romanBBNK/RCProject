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
#include <sys/stat.h>
#include <errno.h>
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
		printf("%s", ptr);
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
		} else {
			n = read(fd, buffer, BUFFERSIZE);
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
		if (toSend < BUFFERSIZE){
			n = read(fd, buffer, toSend);
		} else {
			n = read(fd, buffer, BUFFERSIZE);
		}
		if(n == -1)
			exit(1);
		fwrite(buffer, 1, n, f);
		printf("->>>\n");
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
			//printf("\nREADbuffer->//INICIO// %s //FIM//\n",buffer);
			//n = write(fd, buffer, toSend);
			writeTokenToServer(fd, n, buffer);
		} else {
			n=BUFFERSIZE;
			fread(buffer, 1, BUFFERSIZE, f);
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

int generateFilePath(char* topic, char* question, char* answer, char *imgExt, char *targetPath){

    //Sets base dir and topic
    strcpy(targetPath, "./");
    strcat(targetPath, topic);

    //Creates folder if nonexistent
    errno = 0;
    if( mkdir(targetPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) !=0 && errno != EEXIST){
        printf("Error creating topic folder.\n");
        return -1;
    }

    //Checks if we want a path to a question
    if(question==NULL)
        return 0;
    else{
        strcat(targetPath, "/");
        strcat(targetPath, question);
        errno = 0;
        if( mkdir(targetPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) !=0 && errno != EEXIST){
            printf("Error creating question folder.\n");
            return -1;
        }
    }
    //Current path: ./Data/topic/question

    //Checks if we want a path to an answer
    if(answer==NULL){
        strcat(targetPath, "/");
        strcat(targetPath, question);
        strcat(targetPath, ".");
        //Checks if the path is for an image or text
        if(imgExt == NULL){
            //Current path: ./Data/topic/question/question.txt
            strcat(targetPath, "txt");
            return 0;
        } else{
            //Current path: ./Data/topic/question/question.imgExt
            strcat(targetPath, imgExt);
            return 0;
        }
    } else{
        strcat(targetPath, "/");
        strcat(targetPath, answer);
        errno = 0;
        //Current path: ./Data/topic/question/answer
        if( mkdir(targetPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) !=0 && errno != EEXIST){
            printf("Error creating answer folder.\n");
            return -1;
        }
        strcat(targetPath, "/");
        strcat(targetPath, answer);
        strcat(targetPath, ".");
        //Checks if the path is for an image or text
        if(imgExt == NULL){
            //Current path: ./Data/topic/question/answer/answer.txt
            strcat(targetPath, "txt");
            return 0;
        } else{
            //Current path: ./Data/topic/question/answer/answer.imgExt
            strcat(targetPath, imgExt);
            return 0;
        }
    }
}

void question_get(int newfd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *parse, char *topic, char *question) {
	char *extension = (char *)malloc(5*sizeof(char));
	char *answerID = (char *)malloc(2*sizeof(char));
	char *answerName = (char *)malloc(13*sizeof(char));
	char *targetPath = (char *)malloc(100*sizeof(char));

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

	//write to file qdata
	generateFilePath(topic, question, NULL, NULL, targetPath);
	writeToFile(newfd, n, buffer, targetPath, qsize);
	memset(targetPath, '\0', sizeof(char)*100);

	//le " "
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

		//isize
		readTokenFromServer(newfd, n, buffer);

		int qisize = atol(buffer);

		printf("qisize -> %d\n", qisize);

		generateFilePath(topic, question, NULL, extension, targetPath);
		writeToFile2(newfd, n, buffer, targetPath, qisize);
		memset(targetPath, '\0', sizeof(char)*100);

		//le " "
		readTokenFromServer(newfd, n, buffer);
	}

	//N
	readTokenFromServer(newfd, n, buffer);

	int n_ans = atol(buffer);

	printf("N -> %d\n", n_ans);
	
	char* aUserID = (char *)malloc(5*sizeof(char));
	for (int i=0; i<n_ans; i++){
		//AN
		readTokenFromServer(newfd, n, buffer);
		strcpy(answerID, buffer);
		strcpy(answerName, question);
		strcat(answerName, "_");
		strcat(answerName, answerID);
		
		printf("AN -> %s\n", answerID);

		//aUserID
		readTokenFromServer(newfd, n, buffer);

		strcpy(aUserID, buffer);
		printf("aUserID -> %s\n", aUserID);

		//asize
		readTokenFromServer(newfd, n, buffer);

		int asize = atol(buffer);

		printf("asize -> %d\n", asize);

		generateFilePath(topic, question, answerName, NULL, targetPath);
		writeToFile(newfd, n, buffer, targetPath, asize);
		memset(targetPath, '\0', sizeof(char)*100);

		//le " "
		readTokenFromServer(newfd, n, buffer);

		readTokenFromServer(newfd, n, buffer);

		int aIMG = atol(buffer);
		
		printf("aIMG -> %d\n", aIMG);
		if (aIMG){
			//aiext
			readTokenFromServer(newfd, n, buffer);

			strcpy(extension, buffer);

			printf("aiext -> %s\n", extension);

			//aisize
			readTokenFromServer(newfd, n, buffer);

			int aisize = atol(buffer);

			printf("aisize -> %d\n", aisize);

			generateFilePath(topic, question, answerName, extension, targetPath);
			writeToFile2(newfd, n, buffer, targetPath, aisize);
			memset(targetPath, '\0', sizeof(char)*100);
			//le "\n"
			readTokenFromServer(newfd, n, buffer);
		}
	}
	free(extension);
	free(answerID);
	free(targetPath);
	free(aUserID);
	free(qUserID);
	free(answerName);
}

void question_submit(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *parse, char *userID, char *topic, char *question, char *textFile, char *imageFile){
	char *qSize = (char *)malloc(10*sizeof(char));
	char *iSize = (char *)malloc(10*sizeof(char));

	//write the user request to server
	strcat(buffer, "QUS ");
	strcat(buffer, userID);
	strcat(buffer, " ");
	strcat(buffer, topic);
	strcat(buffer, " ");
	strcat(buffer, question);
	strcat(buffer, " ");
	int size = sizeOfFile(textFile);
	sprintf(qSize, "%d", size);
	strcat(buffer, qSize);
	strcat(buffer, " ");

	writeTokenToServer(fd, n, buffer);
	writeFromFile(fd, n, buffer, textFile, size);

	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

	if (imageFile == NULL){
		strcat(buffer, " 0");
		writeTokenToServer(fd, n, buffer);
	} else {
		strcat(buffer, " 1 ");
		char *end = imageFile;
		while(end < imageFile + strlen(imageFile)) {
			if (*end == '.'){
				end++;
				strcat(buffer, end);
			}
			end++;
		}
		strcat(buffer, " ");
		size = sizeOfFile2(imageFile);
		sprintf(iSize, "%d", size);
		strcat(buffer, iSize);
		strcat(buffer, " ");

		writeTokenToServer(fd, n, buffer);

		writeFromFile2(fd, n, buffer, imageFile, size);
	}
	strcat(buffer, "\n");
	writeTokenToServer(fd, n, buffer);

	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
	
	//read info from the server token by token
	readTokenFromServer(fd, n, buffer);
	readTokenFromServer(fd, n, buffer);

	if ((strcmp(buffer, "OK") == 0)) {
		write(1, "Question was successfully created\n", 34);
	} else if ((strcmp(buffer, "DUP") == 0)) {
		write(1, "Question already exists\n", 24);
	} else if ((strcmp(buffer, "FUL") == 0)) {
		write(1, "Question list is full\n", 22);
	} else {
		write(1, "Question has not been created\n", 30);
	}
	free(qSize);
	free(iSize);
}

void answer_submit(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *parse, char *userID, char *topic, char *question, char *textFile, char *imageFile){
	int size;
	char *aSize = (char *)malloc(10*sizeof(char));
	char *iSize = (char *)malloc(10*sizeof(char));

	//write the user request to server
	strcat(buffer, "ANS ");
	strcat(buffer, userID);
	strcat(buffer, " ");
	strcat(buffer, topic);
	strcat(buffer, " ");
	strcat(buffer, question);
	strcat(buffer, " ");
	size = sizeOfFile(textFile);
	sprintf(aSize, "%d", size);
	strcat(buffer, aSize);
	strcat(buffer, " ");

	writeTokenToServer(fd, n, buffer);
	writeFromFile(fd, n, buffer, textFile, size);
	
	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

	if (imageFile == NULL){
		strcat(buffer, " 0");
		writeTokenToServer(fd, n, buffer);
	} else {
		strcat(buffer, " 1 ");
		char *end = imageFile;
		while(end < imageFile + strlen(imageFile)) {
			if (*end == '.'){
				end++;
				strcat(buffer, end);
			}
			end++;
		}
		strcat(buffer, " ");
		size = sizeOfFile2(imageFile);
		sprintf(iSize, "%d", size);
		strcat(buffer, iSize);
		strcat(buffer, " ");

		writeTokenToServer(fd, n, buffer);

		writeFromFile2(fd, n, buffer, imageFile, size);
	}
	strcat(buffer, "\n");
	writeTokenToServer(fd, n, buffer);

	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

	//read info from the server token by token
	readTokenFromServer(fd, n, buffer);
	printf("%s", buffer);
	readTokenFromServer(fd, n, buffer);
	printf("%s\n", buffer);
	
	if ((strcmp(buffer, "OK") == 0)) {
		write(1, "Answer was successfully created\n", 32);
	} else if ((strcmp(buffer, "FUL") == 0)) {
		write(1, "Answer list is full\n", 20);
	} else {
		write(1, "Answer has not been created\n", 28);
	}

	free(aSize);
	free(iSize);
}