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

char* port;

static void parseArgs(long argc, char* const argv[]){
	long opt;
	opterr = 0;

	port = "58000";
	while ( (opt = getopt(argc, argv, "p:")) != -1){
		switch (opt){
			
			case 'p':
				port = optarg;
				break;
			case '?':
			default:
				//opterr++;
				port = "58000";
				break;
		}
	}
}

void validReg(int fd, int addrlen, int n, struct sockaddr_in addr, char *buffer, char *parse) {
	parse = strtok(NULL, " ");

	write(1,"REG",3);
	write(1, "\n", 1);

	if( 10000 < atoi(parse)  && atoi(parse) < 99999 ) {
		n=sendto(fd,"OK",2,0,(struct sockaddr*) &addr, addrlen);
	}
	else {
		n=sendto(fd,"NOK",3,0,(struct sockaddr*) &addr, addrlen);
	}
	if(n==-1)
		exit(1);
}

void topic_list(int fd, int addrlen, int n, struct sockaddr_in addr, char *buffer, char *parse) {
	//////// Lista de tópicos ////////////
	write(1,"Lista de topicos",16);
	write(1, "\n", 1);
	//////// Lista de tópicos ////////////
	n=sendto(fd,"Lista de topicos",16,0,(struct sockaddr*) &addr, addrlen);
	if(n==-1)
		exit(1);
}

void topic_propose(int fd, int addrlen, int n, struct sockaddr_in addr, char *buffer, char *parse) {
	parse = strtok(NULL, " "); // parse = userID
	parse = strtok(NULL, " "); // parse = topic
	
	n=sendto(fd,"OK",2,0,(struct sockaddr*) &addr, addrlen);
}

void question_list(int fd, int addrlen, int n, struct sockaddr_in addr, char *buffer, char *parse) {
	parse = strtok(NULL, " "); // parse = topic
	
	n=sendto(fd,"OK",2,0,(struct sockaddr*) &addr, addrlen);
}

int main(int argc, char *argv[]){

	int fd, addrlen, n;
	struct addrinfo hints, *res;
	struct sockaddr_in addr;
	char *buffer = (char *)malloc(128*sizeof(char));

	memset(&hints, 0, sizeof hints);
	hints.ai_family=AF_INET;
	hints.ai_socktype=SOCK_DGRAM;
	hints.ai_flags=AI_PASSIVE|AI_NUMERICSERV;

	parseArgs(argc, (char** const)argv);

	n=getaddrinfo(NULL,port,&hints,&res);

	if(n!=0)
		exit(1);

	fd=socket(res->ai_family,res->ai_socktype,res->ai_protocol);
	if(fd==-1)
		exit(1);

	n=bind(fd,res->ai_addr,res->ai_addrlen);
	if(n==-1)
		exit(1);

	addrlen=sizeof(addr);

	char *parse;

	int max = fd + 1;
	fd_set rset;
	FD_ZERO(&rset);

	while(1) {

		memset(buffer, '\0', sizeof(char)*128);

		FD_SET(fd, &rset);

		select(max, &rset, NULL, NULL, NULL);

		if(FD_ISSET(fd, &rset)) {
			n=recvfrom(fd,buffer,128,0,(struct sockaddr*) &addr, &addrlen);
			if(n==-1)
				exit(1);
		}

		parse = strtok(buffer, " ");

		if ((strcmp(parse, "REG") == 0)){
			validReg(fd, addrlen, n, addr, buffer, parse);
		} else if ((strcmp(parse, "LTP") == 0)){
			topic_list(fd, addrlen, n, addr, buffer, parse);
		} else if ((strcmp(parse, "PTP") == 0)){
			topic_propose(fd, addrlen, n, addr, buffer, parse);
		} else if ((strcmp(parse, "LQU") == 0)){
			question_list(fd, addrlen, n, addr, buffer, parse);
		}
	}

	free(buffer);
	free(parse);
	freeaddrinfo(res);
	close(fd);

	return 0;
}
