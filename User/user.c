#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
//#define PORT "58000"
#define BUFFERSIZE 1000

char* ip;
char* port;

static void parseArgs(long argc, char* const argv[]){
	long opt;
	opterr = 0;

	while ( (opt = getopt(argc, argv, "n:p:")) != -1){
		switch (opt){
			
			case 'n':
				ip = optarg;
				break;
			case 'p':
				port = optarg;
				break;
			case '?':
			default:
				break;
		}
	}
}

void reg(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *parse, char userID[]) {
	n = sendto(fd, buffer, strlen(buffer), 0, res->ai_addr, res->ai_addrlen);
	if(n == -1)
		exit(1);

	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

	addrlen = sizeof(addr);
	n = recvfrom(fd, buffer, BUFFERSIZE, 0, (struct sockaddr*) &addr, &addrlen);
	if(n == -1)
		exit(1);

	parse = strtok(buffer, " ");
	parse = strtok(NULL, "\n");

	if ((strcmp(parse, "OK") == 0)) {
		write(1, "User \"", 6);
		write(1, userID, 5);
		write(1, "\" registered\n", 13);
	} else {
		write(1, "User not registered\n", 20);
	}
}

void topic_list(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char buffer[]) {
	n = sendto(fd, buffer, strlen(buffer), 0, res->ai_addr, res->ai_addrlen);
	if(n == -1)
		exit(1);

	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

	addrlen = sizeof(addr);
	n = recvfrom(fd, buffer, BUFFERSIZE, 0, (struct sockaddr*) &addr, &addrlen);
	if(n == -1)
		exit(1);

	write(1, buffer, n);
	write(1, "\n", 1);
}

void topic_propose(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *parse, char userID[], char *topic) {
	scanf("%s", topic);
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

void question_list(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *parse, char *topic) {
	scanf("%s", topic);
	strcat(buffer, "LQU ");
	strcat(buffer, topic);
	strcat(buffer, "\n");

	n = sendto(fd, buffer, strlen(buffer), 0, res->ai_addr, res->ai_addrlen);
	if(n == -1)
		exit(1);

	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
	memset(topic, '\0', sizeof(char)*10);

	addrlen = sizeof(addr);
	n = recvfrom(fd, buffer, BUFFERSIZE, 0, (struct sockaddr*) &addr, &addrlen);
	if(n == -1)
		exit(1);

	write(1, buffer, n);
	write(1, "\n", 1);
}

int main(int argc, char *argv[]){

	//UDP inicialization
	int fd, addrlen, n;
	struct addrinfo hints, *res;
	struct sockaddr_in addr;
	char *buffer = (char *)malloc(BUFFERSIZE*sizeof(char));
	char *topic = (char *)malloc(10*sizeof(char));
	char *topic_number = (char *)malloc(2*sizeof(char));
	memset(&hints, 0, sizeof hints);
	hints.ai_family=AF_INET;
	hints.ai_socktype=SOCK_DGRAM;
	hints.ai_flags=AI_NUMERICSERV;

	char hostname[128];
	gethostname(hostname, 128);
	ip = hostname;
	port = "58000";

	parseArgs(argc, (char** const)argv);

	n = getaddrinfo(ip, port, &hints, &res);
	if(n != 0)
		exit(1);

	fd=socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if(fd == -1)
		exit(1);

	char *parse;

	char command[100];
	char userID[5];

	scanf("%s", command);
	while (strcmp(command, "exit") != 0){

		memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

		if ( (strcmp(command, "register") == 0) || (strcmp(command, "reg") == 0) ){
			scanf("%s", userID);
			strcat(buffer, "REG ");
			strcat(buffer, userID);
			strcat(buffer, "\n");
			reg(fd, addrlen, n, res, addr, buffer, parse, userID);

		} else if ( (strcmp(command, "topic_list") == 0) || (strcmp(command, "tl") == 0)){
			strcat(buffer, "LTP ");
			strcat(buffer, "\n");
			topic_list(fd, addrlen, n, res, addr, buffer);
		} else if (strcmp(command, "topic_select") == 0){
			memset(topic, '\0', sizeof(char)*10);
			memset(topic_number, '\0', sizeof(char)*2);
			scanf("%s", topic);
		} else if (strcmp(command, "ts") == 0){
			memset(topic, '\0', sizeof(char)*10);
			memset(topic_number, '\0', sizeof(char)*2);
			scanf("%s", topic_number);
		} else if ( (strcmp(command, "topic_propose") == 0) || (strcmp(command, "tp") == 0)){
			topic_propose(fd, addrlen, n, res, addr, buffer, parse, userID, topic);
		} else if ( (strcmp(command, "question_list") == 0) || (strcmp(command, "ql") == 0)){
			if (strlen(topic) > 0)
				question_list(fd, addrlen, n, res, addr, buffer, parse, topic);
			else if (strlen(topic_number) > 0)
				question_list(fd, addrlen, n, res, addr, buffer, parse, topic_number);
			else
				write(1,"Topic not defined yet\n", 22);
		} else if (strcmp(command, "question_get") == 0){
			
		} else if (strcmp(command, "qg") == 0){
			
		} else if ( (strcmp(command, "question_submit") == 0) || (strcmp(command, "qs") == 0)){
			
		} else if ( (strcmp(command, "answer_submit") == 0) || (strcmp(command, "as") == 0)){
			
		}
		scanf("%s", command);
	}

	free(topic_number);
	free(topic);
	free(buffer);
	freeaddrinfo(res);
	close(fd);

	return 0;
}
