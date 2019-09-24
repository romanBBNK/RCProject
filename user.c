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

void reg(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char buffer[], char userID[]) {
	n = sendto(fd, "reg", 4, 0, res->ai_addr, res->ai_addrlen);
	if(n == -1)
		exit(1);

	n = sendto(fd, userID, 5, 0, res->ai_addr, res->ai_addrlen);
	if(n == -1)
		exit(1);

	addrlen = sizeof(addr);
	n = recvfrom(fd, buffer, 128, 0, (struct sockaddr*) &addr, &addrlen);
	if(n == -1)
		exit(1);

	write(1, buffer, n);
	write(1, "\n", 1);
}

void tl(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char buffer[]) {
	n = sendto(fd, "tl", 3, 0, res->ai_addr, res->ai_addrlen);
	if(n == -1)
		exit(1);
}

void tp(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char buffer[], char userID[]) {
	n = sendto(fd, "tp", 3, 0, res->ai_addr, res->ai_addrlen);
	if(n == -1)
		exit(1);

	n = sendto(fd, userID, 5, 0, res->ai_addr, res->ai_addrlen);
	if(n == -1)
		exit(1);

	scanf("%s", buffer);
	//PROVAVELMENTE ISTO VAI DAR ERRO NO FUTURO STRLEN(BUFFER)
	n = sendto(fd, buffer, strlen(buffer), 0, res->ai_addr, res->ai_addrlen);
	//PROVAVELMENTE ISTO VAI DAR ERRO NO FUTURO STRLEN(BUFFER)
	if(n == -1)
		exit(1);

	addrlen = sizeof(addr);
	n = recvfrom(fd, buffer, 128, 0, (struct sockaddr*) &addr, &addrlen);
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
	char buffer[128];
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

	char command[100];
	char userID[5];

	scanf("%s", command);
	while (strcmp(command, "exit") != 0){

		if ( (strcmp(command, "register") == 0) || (strcmp(command, "reg") == 0) ){
			scanf("%s", userID);
			reg(fd, addrlen, n, res, addr, buffer, userID);

		} else if ( (strcmp(command, "topic_list") == 0) || (strcmp(command, "tl") == 0)){
			tl(fd, addrlen, n, res, addr, buffer);
		} else if (strcmp(command, "topic_select") == 0){
			
		} else if (strcmp(command, "ts") == 0){
			
		} else if ( (strcmp(command, "topic_propose") == 0) || (strcmp(command, "tp") == 0)){
			tp(fd, addrlen, n, res, addr, buffer, userID);
		} else if ( (strcmp(command, "question_list") == 0) || (strcmp(command, "ql") == 0)){
			
		} else if (strcmp(command, "question_get") == 0){
			
		} else if (strcmp(command, "qg") == 0){
			
		} else if ( (strcmp(command, "question_submit") == 0) || (strcmp(command, "qs") == 0)){
			
		} else if ( (strcmp(command, "answer_submit") == 0) || (strcmp(command, "as") == 0)){
			
		}
		scanf("%s", command);
	}



/*
	n = sendto(fd, "Hello!\n", 7, 0, res->ai_addr, res->ai_addrlen);
	if(n == -1)
		exit(1);

	addrlen = sizeof(addr);
	n = recvfrom(fd, buffer, 128, 0, (struct sockaddr*) &addr, &addrlen);
	if(n == -1)
		exit(1);

	write(1, "echo: ", 6);
	write(1, buffer, n);
*/

	freeaddrinfo(res);
	close(fd);

	return 0;
}
