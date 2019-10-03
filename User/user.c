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
#include "tcp.h"
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