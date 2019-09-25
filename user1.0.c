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

void reg(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer) {
	n = sendto(fd, buffer, strlen(buffer), 0, res->ai_addr, res->ai_addrlen);
	if(n == -1)
		exit(1);

	memset(buffer, '\0', sizeof(char)*128);

	addrlen = sizeof(addr);
	n = recvfrom(fd, buffer, 128, 0, (struct sockaddr*) &addr, &addrlen);
	if(n == -1)
		exit(1);

	write(1, buffer, n);
	write(1, "\n", 1);
}

void topic_list(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char buffer[]) {
	n = sendto(fd, buffer, strlen(buffer), 0, res->ai_addr, res->ai_addrlen);
	if(n == -1)
		exit(1);

	memset(buffer, '\0', sizeof(char)*128);

	addrlen = sizeof(addr);
	n = recvfrom(fd, buffer, 128, 0, (struct sockaddr*) &addr, &addrlen);
	if(n == -1)
		exit(1);

	write(1, buffer, n);
	write(1, "\n", 1);
}

void topic_propose(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char userID[], char *topic) {
	scanf("%s", topic);
	strcat(buffer, "PTP ");
	strcat(buffer, userID);
	strcat(buffer, " ");
	strcat(buffer, topic);
	strcat(buffer, "\n");

	n = sendto(fd, buffer, strlen(buffer), 0, res->ai_addr, res->ai_addrlen);
	if(n == -1)
		exit(1);

	memset(buffer, '\0', sizeof(char)*128);
	memset(topic, '\0', sizeof(char)*10);

	addrlen = sizeof(addr);
	n = recvfrom(fd, buffer, 128, 0, (struct sockaddr*) &addr, &addrlen);
	if(n == -1)
		exit(1);

	write(1, buffer, n);
	write(1, "\n", 1);
}

void question_list(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *topic) {
	scanf("%s", topic);
	strcat(buffer, "LQU ");
	strcat(buffer, topic);
	strcat(buffer, "\n");

	n = sendto(fd, buffer, strlen(buffer), 0, res->ai_addr, res->ai_addrlen);
	if(n == -1)
		exit(1);

	memset(buffer, '\0', sizeof(char)*128);
	memset(topic, '\0', sizeof(char)*10);

	addrlen = sizeof(addr);
	n = recvfrom(fd, buffer, 128, 0, (struct sockaddr*) &addr, &addrlen);
	if(n == -1)
		exit(1);

	write(1, buffer, n);
	write(1, "\n", 1);
}

void connectTCP(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr){
	n=connect(fd,res->ai_addr,res->ai_addrlen);
	if(n==-1)
		exit(1);
}

void question_get(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char buffer[], char topic[], char question[]) {
	connectTCP(fd, addrlen, n, res, addr);

	n = write(fd, "GQU ", 4);
	if(n == -1)
		exit(1);
	n = write(fd, topic, strlen(topic));
	if(n == -1)
		exit(1);
	n = write(fd, question, strlen(question));
	if(n == -1)
		exit(1);
	n = write(fd, "\n", 1);
	if(n == -1)
		exit(1);
	n = read(fd,"", 128);//
	if(n == -1)
		exit(1);
	
}

void qg(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char buffer[], char topic[], int question_number) {
	connectTCP(fd, addrlen, n, res, addr);

	n = write(fd, "GQU ", 4);
	if(n == -1)
		exit(1);
	n = write(fd, topic, strlen(topic));
	if(n == -1)
		exit(1);
	n = write(fd, question_number, 2);
	if(n == -1)
		exit(1);
	n = write(fd, "\n", 1);
	if(n == -1)
		exit(1);
	n = read(fd, "", 128);//
	if(n == -1)
		exit(1);
	
}

void question_submit(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char buffer[], char topic[], 
					char question[], char qsize[], char qdata[], int qIMG, char iext[], char isize[], char idata[]){
	connectTCP(fd, addrlen, n, res, addr);

	n = write(fd, "QUS ", 4);
	if(n == -1)
		exit(1);
	n = write(fd, userID, 5);
	if(n == -1)
		exit(1);
	n = write(fd, topic, strlen(topic));
	if(n == -1)
		exit(1);
	n = write(fd, question, strlen(question));
	if(n == -1)
		exit(1);
	n = write(fd, qsize, strlen(qsize));
	if(n == -1)
		exit(1);
	n = write(fd, qdata, strlen(qdata));
	if(n == -1)
		exit(1);
	n = write(fd, qIMG, 1);
	if(n == -1)
		exit(1);
	if (qIMG == 1){
		n = write(fd, iext, strlen(iext));
		if(n == -1)
			exit(1);
		n = write(fd, isize, strlen(isize));
		if(n == -1)
			exit(1);
		n = write(fd, idata, strlen(idata));
		if(n == -1)
			exit(1);
	}
	n = write(fd, "\n", 1);
	if(n == -1)
		exit(1);
	n = read(fd, "", 128);//
	if(n == -1)
		exit(1);
}

void answer_submit(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char buffer[], char topic[], 
					char question[], char asize[], char adata[], int aIMG, char iext[], char isize[], char idata[]){
	connectTCP(fd, addrlen, n, res, addr);

	n = write(fd, "ANS ", 4);
	if(n == -1)
		exit(1);
	n = write(fd, userID, 5);
	if(n == -1)
		exit(1);
	n = write(fd, topic, strlen(topic));
	if(n == -1)
		exit(1);
	n = write(fd, question, strlen(question));
	if(n == -1)
		exit(1);
	n = write(fd, asize, strlen(asize));
	if(n == -1)
		exit(1);
	n = write(fd, adata, strlen(adata));
	if(n == -1)
		exit(1);
	n = write(fd, aIMG, 1);
	if(n == -1)
		exit(1);
	if (qIMG == 1){
		n = write(fd, iext, strlen(iext));
		if(n == -1)
			exit(1);
		n = write(fd, isize, strlen(isize));
		if(n == -1)
			exit(1);
		n = write(fd, idata, strlen(idata));
		if(n == -1)
			exit(1);
	}
	n = write(fd, "\n", 1);
	if(n == -1)
		exit(1);
	n = read(fd, "", 128);//
	if(n == -1)
		exit(1);
}

int main(int argc, char *argv[]){

	//UDP inicialization
	int fd, addrlen, n;
	struct addrinfo hints, *res;
	struct sockaddr_in addr;
	char *buffer = (char *)malloc(128*sizeof(char));
	char *topic = (char *)malloc(10*sizeof(char));
	memset(&hints, 0, sizeof hints);
	hints.ai_family=AF_INET;
	hints.ai_socktype=SOCK_DGRAM;
	hints.ai_flags=AI_NUMERICSERV;

	//TCP inicialization
	int fdTCP, addrlenTCP, nTCP;
	struct addrinfo hintsTCP, *resTCP;
	struct sockaddr_in addrTCP;
	memset(&hintsTCP, 0, sizeof hintsTCP);
	hintsTCP.ai_family=AF_INET;
	hintsTCP.ai_socktype=SOCK_STREAM;
	hintsTCP.ai_flags=AI_NUMERICSERV;

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

	char command[100];
	char userID[5];
	char *question = (char *)malloc(10*sizeof(char));
	int question_number;
	char text_file[128];
	char image_file[128];
	char *qsize;
	char *qdata;
	int qIMG;
	char *asize;
	char *adata;
	int aIMG;

	scanf("%s", command);
	while (strcmp(command, "exit") != 0){

		memset(buffer, '\0', sizeof(char)*128);

		if ( (strcmp(command, "register") == 0) || (strcmp(command, "reg") == 0) ){
			scanf("%s", userID);
			strcat(buffer, "REG ");
			strcat(buffer, userID);
			strcat(buffer, "\n");
			reg(fd, addrlen, n, res, addr, buffer);

		} else if ( (strcmp(command, "topic_list") == 0) || (strcmp(command, "tl") == 0)){
			strcat(buffer, "LTP ");
			strcat(buffer, "\n");
			topic_list(fd, addrlen, n, res, addr, buffer);
		} else if (strcmp(command, "topic_select") == 0){
			
		} else if (strcmp(command, "ts") == 0){
			
		} else if ( (strcmp(command, "topic_propose") == 0) || (strcmp(command, "tp") == 0)){
			topic_propose(fd, addrlen, n, res, addr, buffer, userID, topic);
		} else if ( (strcmp(command, "question_list") == 0) || (strcmp(command, "ql") == 0)){
			question_list(fd, addrlen, n, res, addr, buffer, topic);
		} else if (strcmp(command, "question_get") == 0){
			scanf("%s", question);
			question_get(fdTCP, addrlenTCP, nTCP, resTCP, addrTCP, buffer, topic, question);
		} else if (strcmp(command, "qg") == 0){
			scanf("%d", question_number);
			qg(fdTCP, addrlenTCP, nTCP, resTCP, addrTCP, buffer, topic, question_number);
		} else if ( (strcmp(command, "question_submit") == 0) || (strcmp(command, "qs") == 0)){
			scanf("%s", qsize);
			scanf("%s", qdata);
			scanf("%d", qIMG);
			if (qIMG == 1){
				scanf("%s", iext);
				scanf("%s", isize);
				scanf("%s", idata);
			}
			question_submit(fdTCP, addrlenTCP, nTCP, resTCP, addrTCP, buffer, topic, question, qsize, qdata, qIMG, iext, isize, idata);
		} else if ( (strcmp(command, "answer_submit") == 0) || (strcmp(command, "as") == 0)){
			scanf("%s", asize);
			scanf("%s", adata);
			scanf("%d", aIMG);
			if (qIMG == 1){
				scanf("%s", iext);
				scanf("%s", isize);
				scanf("%s", idata);
			answer_submit(fdTCP, addrlenTCP, nTCP, resTCP, addrTCP, buffer, topic, question, asize, adata, aIMG, iext, isize, idata);
		}
		scanf("%s", command);
	}

	free(topic);
	free(buffer);
	free(question);
	freeaddrinfo(res);
	close(fd);

	return 0;
}
