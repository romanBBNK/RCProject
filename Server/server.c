#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include "udpServer.h"
#include "tcpServer.h"
#include "data.h"
//#define PORT "58000"
#define BUFFERSIZE 128
#define MAXMSGSIZE 2048

/*
 * Global variables to be used.
 */


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


int main(int argc, char *argv[]){

	topicList = NULL;
	topicListEnd = NULL;
	topic_counter = 0;

	int fd, addrlen, n;
	struct addrinfo hints, *res;
	struct sockaddr_in addr;
	char *buffer = (char *)malloc(BUFFERSIZE*sizeof(char));
	char count[2] = "";

	memset(&hints, 0, sizeof hints);
	hints.ai_family=AF_INET;
	hints.ai_socktype=SOCK_DGRAM;
	hints.ai_flags=AI_PASSIVE|AI_NUMERICSERV;

	int fdTCP, newfd, addrlenTCP, nTCP;
	struct addrinfo hintsTCP, *resTCP;
	struct sockaddr_in addrTCP;

	memset(&hintsTCP, 0, sizeof hintsTCP);
	hintsTCP.ai_family=AF_INET;
	hintsTCP.ai_socktype=SOCK_STREAM;
	hintsTCP.ai_flags=AI_PASSIVE|AI_NUMERICSERV;

	retrieveStoredData();

	parseArgs(argc, (char** const)argv);

	/*
	 * TODO: Add function calls to retrieve stored data into the file structures.
	 */

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

	/*
	fdTCP=socket(res->ai_family,res->ai_socktype,res->ai_protocol);
	if(fdTCP==-1)
		exit(1);

	nTCP=bind(fdTCP,res->ai_addr,res->ai_addrlen);
	if(nTCP==-1)
		exit(1);

	addrlenTCP=sizeof(addrTCP);
*/ 

	while(1) {

		memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

		FD_SET(fd, &rset);

		select(max, &rset, NULL, NULL, NULL);

		if(FD_ISSET(fd, &rset)) {
			n=recvfrom(fd,buffer,BUFFERSIZE,0,(struct sockaddr*) &addr, &addrlen);
			if(n==-1)
				exit(1);
		} else if (FD_ISSET(fd, &rset)){
			if(listen(fd,5)==-1)
				exit(1);
			if((newfd=accept(fd,(struct sockaddr*) &addr, &addrlen)) == -1)
				exit(1);
			n=read(newfd,buffer,strlen(buffer));
			if(n==-1)
				exit(1);
		}

		parse = strtok(buffer, " \n");

		if ((strcmp(parse, "REG") == 0)){
			validReg(fd, addrlen, n, addr, buffer, parse);
		} else if ((strcmp(parse, "LTP") == 0)){
			topic_list(fd, addrlen, n, addr, buffer, count, topic_counter);
		} else if ((strcmp(parse, "PTP") == 0)){
			topic_propose(fd, addrlen, n, addr, buffer, parse);
		} else if ((strcmp(parse, "LQU") == 0)){
			question_list(fd, addrlen, n, addr, buffer, parse);
		} else if ((strcmp(parse, "GQU") == 0)){
			question_get(fdTCP, addrlenTCP, nTCP, buffer, parse, newfd);
		} else if ((strcmp(parse, "QUS") == 0)){
			question_submit(fdTCP, addrlenTCP, nTCP, buffer, parse, newfd);
		} else if ((strcmp(parse, "ANS") == 0)){
			answer_submit(fdTCP, addrlenTCP, nTCP, buffer, parse, newfd);
		}
	}

	free(buffer);
	free(parse);
	freeaddrinfo(res);
	close(fd);

	return 0;
}
