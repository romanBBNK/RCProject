#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/time.h>
#include "udp.h"
#include "tcp.h"
//#define PORT "58000"
#define BUFFERSIZE 1000

char* ip;
char* port;

static void parseArgs(long argc, char* const argv[]){
	char hostname[128];
	gethostname(hostname, 128);
	ip = hostname;
	port = "58000";

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
	char *textFile = (char *)malloc(128*sizeof(char));
	char *imageFile = (char *)malloc(128*sizeof(char));
	memset(&hintsTCP, 0, sizeof hintsTCP);
	hintsTCP.ai_family=AF_INET;
	hintsTCP.ai_socktype=SOCK_STREAM;
	hintsTCP.ai_flags=AI_NUMERICSERV;

	parseArgs(argc, (char** const)argv);

	//n = getaddrinfo(ip, port, &hints, &res);
	n = getaddrinfo("tejo.tecnico.ulisboa.pt", "58011", &hints, &res);
	if(n != 0)
		exit(1);

	fd=socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if(fd == -1)
		exit(1);

	struct timeval timeout={3,0};
	setsockopt(fd,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(struct timeval));

	//nTCP = getaddrinfo(ip, port, &hintsTCP, &resTCP);
	nTCP = getaddrinfo("tejo.tecnico.ulisboa.pt", "58011", &hintsTCP, &resTCP);
	if(nTCP != 0)
		exit(1);
	
	char *parse;
	char *parseTCP = (char *)malloc(BUFFERSIZE*sizeof(char));

	char command[100];

	char c;
	int x;

	memset(userID, '\0', sizeof(char)*5);
	scanf("%s", command);

	while (strcmp(command, "exit") != 0){

		memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
		memset(parseTCP, '\0', sizeof(char)*BUFFERSIZE);

		if ( (strcmp(command, "register") == 0) || (strcmp(command, "reg") == 0) ){
			scanf("%s", userID);
			strcat(buffer, "REG ");
			strcat(buffer, userID);
			strcat(buffer, "\n");
			reg(fd, addrlen, n, res, addr, buffer, parse, userID);

		} else if ( (strcmp(command, "topic_list") == 0) || (strcmp(command, "tl") == 0)){
			strcat(buffer, "LTP");
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
			memset(topic, '\0', sizeof(char)*10);
			scanf("%s", topic);
			question_list(fd, addrlen, n, res, addr, buffer, parse, topic);
		} else if ( (strcmp(command, "question_get") == 0) || (strcmp(command, "qg") == 0)){
			fdTCP=socket(resTCP->ai_family, resTCP->ai_socktype, resTCP->ai_protocol);
			if(fdTCP == -1)
				exit(1);

			nTCP=connect(fdTCP,resTCP->ai_addr,resTCP->ai_addrlen);
			if(nTCP==-1)
				exit(1);
			scanf("%s", question);
			question_get(fdTCP, addrlenTCP, nTCP, resTCP, addrTCP, buffer, parse, topic, question);
		} else if ( (strcmp(command, "question_submit") == 0) || (strcmp(command, "qs") == 0)){
			fdTCP=socket(resTCP->ai_family, resTCP->ai_socktype, resTCP->ai_protocol);
			if(fdTCP == -1)
				exit(1);

			nTCP=connect(fdTCP,resTCP->ai_addr,resTCP->ai_addrlen);
			if(nTCP==-1)
				exit(1);
			x=0;
			getchar();
			while ((c=getchar()) != '\n'){
				parseTCP[x++]=c;
			}
			question = strtok(parseTCP, " ");
			textFile = strtok(NULL, " ");
			imageFile = strtok(NULL, "\n");
			question_submit(fdTCP, addrlenTCP, nTCP, resTCP, addrTCP, buffer, parse, userID, topic, question, textFile, imageFile);
		} else if ( (strcmp(command, "answer_submit") == 0) || (strcmp(command, "as") == 0)){
			fdTCP=socket(resTCP->ai_family, resTCP->ai_socktype, resTCP->ai_protocol);
			if(fdTCP == -1)
				exit(1);

			nTCP=connect(fdTCP,resTCP->ai_addr,resTCP->ai_addrlen);
			if(nTCP==-1)
				exit(1);
			x=0;
			getchar();
			while ((c=getchar()) != '\n'){
				parseTCP[x++]=c;
			}
			textFile = strtok(parseTCP, " ");
			imageFile = strtok(NULL, "\n");
			answer_submit(fdTCP, addrlenTCP, nTCP, resTCP, addrTCP, buffer, parse, userID, topic, question, textFile, imageFile);
		} /*else if ( (strcmp(command, "ola") == 0) || (strcmp(command, "ola2") == 0)){
			fdTCP=socket(resTCP->ai_family, resTCP->ai_socktype, resTCP->ai_protocol);
			if(fdTCP == -1)
				exit(1);

			nTCP=connect(fdTCP,resTCP->ai_addr,resTCP->ai_addrlen);
			if(nTCP==-1)
				exit(1);

			strcpy(buffer, "UM DOIS TRES QUATRO CINCO SEIS SETE OITO 4952379754692386542954867239548627459846592865479284657928364398649857y298t5294y5g23yu54g3495t3548gi23u5yiu45yg2i3u54yg239548g746u245i23yu54i2u34y52u3h54g2i3u54hg23ui54hg\n");
			
			printf("enviar -> %s\n", buffer);

			int toSend = strlen(buffer);
			while(toSend > 0){
				nTCP = write(fdTCP, buffer, toSend);
				if(n == -1)
					exit(1);
				toSend -= nTCP;
				buffer += nTCP;
			}
		}*/
		scanf("%s", command);
	}

	free(userID);
	free(topicList);
	free(topic);
	free(buffer);
	freeaddrinfo(res);
	close(fd);
	close(fdTCP);

	return 0;
}
