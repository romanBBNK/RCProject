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

int sizeOfFile(char* name){
	FILE* f;
	int size;

	f = fopen(name, "r");
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);
	return size;
}

void reg(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *parse, char *userID) {
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

void topic_list(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *parse, char **topicList) {
	n = sendto(fd, buffer, strlen(buffer), 0, res->ai_addr, res->ai_addrlen);
	if(n == -1)
		exit(1);

	memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

	addrlen = sizeof(addr);
	n = recvfrom(fd, buffer, BUFFERSIZE, 0, (struct sockaddr*) &addr, &addrlen);
	if(n == -1)
		exit(1);

	write(1, "available topics:\n", 18);

	int i = 0;
	parse = strtok(buffer, " ");
	parse = strtok(NULL, " ");
	int size = atoi(parse);
	while(i < size) {
		i++;
		sprintf(parse, "%d", i);
		write(1, parse, strlen(parse));
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
}

void topic_propose(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *parse, char *userID, char *topic) {
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
}

void connectTCP(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr){
	n=connect(fd,res->ai_addr,res->ai_addrlen);
	if(n==-1)
		exit(1);
}

void question_get(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *topic, char *question) {
	connectTCP(fd, addrlen, n, res, addr);

	strcat(buffer, "GQU ");
	strcat(buffer, topic);
	strcat(buffer, " ");
	strcat(buffer, question);
	strcat(buffer, " \n");
	
	n = write(fd, buffer, BUFFERSIZE);
	if(n == -1)
		exit(1);

	memset(buffer, '\0', sizeof(char)*128);
	memset(topic, '\0', sizeof(char)*10);
	memset(question, '\0', sizeof(char)*10);

	n = read(fd,buffer, BUFFERSIZE);
	if(n == -1)
		exit(1);
	write(1, buffer, n);
}

void qg(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *topic, char *question_number) {
	connectTCP(fd, addrlen, n, res, addr);


	strcat(buffer, "GQU ");
	strcat(buffer, topic);
	strcat(buffer, " ");
	strcat(buffer, question_number);
	strcat(buffer, " \n");
	n = write(fd, buffer, BUFFERSIZE);
	if(n == -1)
		exit(1);
	memset(buffer, '\0', sizeof(char)*128);
	memset(topic, '\0', sizeof(char)*10);
	memset(question_number, '\0', sizeof(char)*10);

	n = read(fd, buffer, BUFFERSIZE);
	if(n == -1)
		exit(1);
	write(1, buffer, n);
}

void question_submit(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *parse, char *userID, 
		char *topic, char *question, char *textFile, char *imageFile){
	connectTCP(fd, addrlen, n, res, addr);

	strcat(buffer, "QUS ");
	strcat(buffer, userID);
	strcat(buffer, " ");
	strcat(buffer, topic);
	strcat(buffer, " ");
	strcat(buffer, question);
	strcat(buffer, " ");
	strcat(buffer, "itoa(sizeOfFile(textFile))");
	strcat(buffer, " ");
	strcat(buffer, "qdata");
	if (imageFile != NULL){
		strcat(buffer, " 1 ");
		strcat(buffer, "iext");
		strcat(buffer, " ");
		strcat(buffer, "isize");
		strcat(buffer, " ");
		strcat(buffer, "idata");
	} else {
		strcat(buffer, " 0");
	}
	strcat(buffer, " \n");

	n = write(fd, buffer, BUFFERSIZE);
	if(n == -1)
		exit(1);

	memset(buffer, '\0', sizeof(char)*128);
	memset(topic, '\0', sizeof(char)*10);
	memset(question, '\0', sizeof(char)*10);

	n = read(fd, buffer, BUFFERSIZE);
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

void answer_submit(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *parse, char *userID, 
		char *topic, char *question, char *textFile, char *imageFile){
	connectTCP(fd, addrlen, n, res, addr);

	strcat(buffer, "ANS ");
	strcat(buffer, userID);
	strcat(buffer, " ");
	strcat(buffer, topic);
	strcat(buffer, " ");
	strcat(buffer, question);
	strcat(buffer, " ");
	strcat(buffer, "asize");
	strcat(buffer, " ");
	strcat(buffer, "adata");
	if (imageFile != NULL){
		strcat(buffer, " 1 ");
		strcat(buffer, "iext");
		strcat(buffer, " ");
		strcat(buffer, "isize");
		strcat(buffer, " ");
		strcat(buffer, "idata");
	} else {
		strcat(buffer, " 0");
	}
	strcat(buffer, " \n");

	n = write(fd, buffer, BUFFERSIZE);
	if(n == -1)
		exit(1);

	memset(buffer, '\0', sizeof(char)*128);
	memset(topic, '\0', sizeof(char)*10);
	memset(question, '\0', sizeof(char)*10);

	n = read(fd, buffer, BUFFERSIZE);
	if(n == -1)
		exit(1);

	parse = strtok(buffer, " ");
	parse = strtok(NULL, "\n");

	if ((strcmp(parse, "OK") == 0)) {
		write(1, "Topic  was successfully created\n", 32);
	} else if ((strcmp(parse, "FUL") == 0)) {
		write(1, "Topic list is full\n", 19);
	} else {
		write(1, "Topic has not been created\n", 27);
	}
}

int main(int argc, char *argv[]){

	//UDP inicialization
	int fd, addrlen, n;
	struct addrinfo hints, *res;
	struct sockaddr_in addr;
	char *buffer = (char *)malloc(BUFFERSIZE*sizeof(char));
	char** topicList = (char**)malloc(99*sizeof(char*));
	char *topic = (char *)malloc(10*sizeof(char));
	int topic_number;
	char *userID = (char *)malloc(5*sizeof(char));
	memset(&hints, 0, sizeof hints);
	hints.ai_family=AF_INET;
	hints.ai_socktype=SOCK_DGRAM;
	hints.ai_flags=AI_NUMERICSERV;

	//TCP inicialization
	int fdTCP, addrlenTCP, nTCP;
	struct addrinfo hintsTCP, *resTCP;
	struct sockaddr_in addrTCP;
	char *question = (char *)malloc(10*sizeof(char));
	char *question_number = (char *)malloc(2*sizeof(char));
	char *textFile = (char *)malloc(128*sizeof(char));
	char *imageFile = (char *)malloc(128*sizeof(char));
	memset(&hintsTCP, 0, sizeof hintsTCP);
	hintsTCP.ai_family=AF_INET;
	hintsTCP.ai_socktype=SOCK_STREAM;
	hintsTCP.ai_flags=AI_NUMERICSERV;

	char hostname[128];
	gethostname(hostname, 128);
	ip = hostname;
	port = "58000";

	parseArgs(argc, (char** const)argv);

	//n = getaddrinfo(ip, port, &hints, &res);
	n = getaddrinfo("tejo.tecnico.ulisboa.pt", "58011", &hints, &res);
	if(n != 0)
		exit(1);

	fd=socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if(fd == -1)
		exit(1);

	//nTCP = getaddrinfo(ip, port, &hints, &res);
	nTCP = getaddrinfo("tejo.tecnico.ulisboa.pt", "58011", &hintsTCP, &resTCP);
	if(nTCP != 0)
		exit(1);

	fdTCP=socket(resTCP->ai_family, resTCP->ai_socktype, resTCP->ai_protocol);
	if(fdTCP == -1)
		exit(1);

	nTCP=connect(fdTCP,resTCP->ai_addr,resTCP->ai_addrlen);
	if(nTCP==-1)
		exit(1);

	char *parse;

	char command[100];

	char c;
	int x;

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
			topic_list(fd, addrlen, n, res, addr, buffer, parse, topicList);
		} else if (strcmp(command, "topic_select") == 0){
			memset(topic, '\0', sizeof(char)*10);
			scanf("%s", topic);
		} else if (strcmp(command, "ts") == 0){
			memset(topic, '\0', sizeof(char)*10);
			scanf("%d", &topic_number);
			strcat(topic, topicList[topic_number]);
		} else if ( (strcmp(command, "topic_propose") == 0) || (strcmp(command, "tp") == 0)){
			memset(topic, '\0', sizeof(char)*10);
			scanf("%s", topic);
			topic_propose(fd, addrlen, n, res, addr, buffer, parse, userID, topic);
		} else if ( (strcmp(command, "question_list") == 0) || (strcmp(command, "ql") == 0)){
			question_list(fd, addrlen, n, res, addr, buffer, parse, topic);
		} else if (strcmp(command, "question_get") == 0){
			scanf("%s", question);
			question_get(fdTCP, addrlenTCP, nTCP, resTCP, addrTCP, buffer, topic, question);
		} else if (strcmp(command, "qg") == 0){
			scanf("%s", question_number);
			qg(fdTCP, addrlenTCP, nTCP, resTCP, addrTCP, buffer, topic, question_number);
		} else if ( (strcmp(command, "question_submit") == 0) || (strcmp(command, "qs") == 0)){
			x=0;
			getchar();
			while ((c=getchar()) != '\n'){
				buffer[x++]=c;
			}
			question = strtok(buffer, " ");
			textFile = strtok(NULL, " ");
			imageFile = strtok(NULL, "\n");
			question_submit(fdTCP, addrlenTCP, nTCP, resTCP, addrTCP, buffer, parse, userID, topic, question, textFile, imageFile);
		} else if ( (strcmp(command, "answer_submit") == 0) || (strcmp(command, "as") == 0)){
			x=0;
			getchar();
			while ((c=getchar()) != '\n'){
				buffer[x++]=c;
			}
			question = strtok(buffer, " ");
			textFile = strtok(NULL, " ");
			imageFile = strtok(NULL, "\n");
			answer_submit(fdTCP, addrlenTCP, nTCP, resTCP, addrTCP, buffer, parse, userID, topic, question, textFile, imageFile);
		}
		scanf("%s", command);
	}

	free(userID);
	free(topicList);
	free(topic_number);
	free(topic);
	free(buffer);
	freeaddrinfo(res);
	close(fd);

	return 0;
}
