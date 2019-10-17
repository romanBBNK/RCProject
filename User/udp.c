#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include "udp.h"
#define BUFFERSIZE 10000

void reg(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *parse, char *userID) {
	n = sendto(fd, buffer, strlen(buffer), 0, res->ai_addr, res->ai_addrlen);
	if(n == -1)
		exit(1);

	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

	addrlen = sizeof(addr);

	n = recvfrom(fd, buffer, BUFFERSIZE, 0, (struct sockaddr*) &addr, &addrlen);
	if (n == -1){
		write(1, "timeout\n", 8);
		return;
	}

	parse = strtok(buffer, " ");
	parse = strtok(NULL, "\n");

	if ((strcmp(parse, "OK") == 0)) {
		write(1, "User \"", 6);
		write(1, userID, 5);
		write(1, "\" registered\n", 13);
	} else {
		memset(userID, '\0', sizeof(char)*5);
		write(1, "User not registered\n", 20);
	}
}

int topic_list(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *parse, char **topicList) {
	n = sendto(fd, buffer, strlen(buffer), 0, res->ai_addr, res->ai_addrlen);
	if(n == -1)
		exit(1);

	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

	addrlen = sizeof(addr);
	n = recvfrom(fd, buffer, BUFFERSIZE, 0, (struct sockaddr*) &addr, &addrlen);
	if (n == -1){
		write(1, "timeout\n", 8);
		return;
	}

	write(1, "available topics:\n", 18);

	int i = 0;
	parse = strtok(buffer, " ");
	parse = strtok(NULL, " ");
	int size = atoi(parse);
	if(size == 0) {
		write(1, "none\n", 5);
	}

	char *num = (char *)malloc(2*sizeof(char));
	while(i < size) {
		i++;
		sprintf(num, "%d", i);
		write(1, num, strlen(num));
		write(1, " - ", 3);
		parse = strtok(NULL, ":");
		write(1, parse, strlen(parse));
		write(1, "(proposed by ", 13);
		topicList[i] = (char*)malloc(sizeof(char)*(strlen(parse) + 1));
		strcpy(topicList[i], parse);
		parse = strtok(NULL, " \n");
		write(1, parse, strlen(parse));
		write(1, ")\n", 2);
	}
	free(num);
	return size;
}

void topic_propose(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *parse, char *userID, char *topic) {
	if(strlen(userID) == 0) {
		write(1, "user not registered\n", 20);
		return;
	}
	strcat(buffer, "PTP ");
	strcat(buffer, userID);
	strcat(buffer, " ");
	strcat(buffer, topic);
	strcat(buffer, "\n");

	n = sendto(fd, buffer, strlen(buffer), 0, res->ai_addr, res->ai_addrlen);
	if(n == -1)
		exit(1);

	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
	memset(topic, '\0', sizeof(char)*10);

	addrlen = sizeof(addr);
	n = recvfrom(fd, buffer, BUFFERSIZE, 0, (struct sockaddr*) &addr, &addrlen);
	if (n == -1){
		write(1, "timeout\n", 8);
		return;
	}

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

int question_list(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *parse, char *topic, char** questionList) {
	strcat(buffer, "LQU ");
	strcat(buffer, topic);
	strcat(buffer, "\n");

	n = sendto(fd, buffer, strlen(buffer), 0, res->ai_addr, res->ai_addrlen);
	if(n == -1)
		exit(1);

	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

	addrlen = sizeof(addr);
	n = recvfrom(fd, buffer, BUFFERSIZE, 0, (struct sockaddr*) &addr, &addrlen);
	if (n == -1){
		write(1, "timeout\n", 8);
		return;
	}

	write(1, "available questions about ", 26);
	
	write(1, topic, strlen(topic));
	write(1,":\n",2);

	int i = 0;
	parse = strtok(buffer, " ");
	parse = strtok(NULL, " ");
	int size = atoi(parse);

	if(size == 0) {
		write(1, "none\n", 5);
	}

	char *num = (char *)malloc(2*sizeof(char));
	while(i < size) {
		i++;
		sprintf(num, "%d", i);
		write(1, num, strlen(num));
		write(1, " - ", 3);
		parse = strtok(NULL, ":");
		write(1, parse, strlen(parse));
		questionList[i] = (char*)malloc(sizeof(char)*(strlen(parse) + 1));
		strcpy(questionList[i], parse);
		parse = strtok(NULL, ":");
		parse = strtok(NULL, " \n");
		write(1, "\n", 1);
	}
	free(num);
	return size;
}