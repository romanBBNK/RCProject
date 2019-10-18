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
#define BUFFERSIZE 10000
#define MAXMSGSIZE 2048

/*
 * Global variables to be used.
 */


static void parseArgs(long argc, char* const argv[]){
	long opt;
	opterr = 0;

	port = "58018";
	while ( (opt = getopt(argc, argv, "p:")) != -1){
		switch (opt){
			
			case 'p':
				port = optarg;
				break;
			case '?':
			default:
				//opterr++;
				port = "58018";
				break;
		}
	}
}


int main(int argc, char *argv[]){

	topicList = NULL;
	topicListEnd = NULL;
	topic_counter = 0;

	int fd, addrlen, n, cnt;
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


	nTCP=getaddrinfo(NULL,port,&hintsTCP,&resTCP);
	
	fdTCP=socket(resTCP->ai_family,resTCP->ai_socktype,resTCP->ai_protocol);
	if(fdTCP==-1)
		exit(1);

	nTCP=bind(fdTCP,resTCP->ai_addr,resTCP->ai_addrlen);
	if(nTCP==-1)
		exit(1);

	if(listen(fdTCP,5)==-1)
		exit(1);
	addrlenTCP=sizeof(addrTCP);
	
	char *parse;
	int max = 1;
	if(fdTCP > fd) {
		max += fdTCP;
	} else {
		max += fd;
	}
	fd_set rset;
	FD_ZERO(&rset);

	pid_t pid;

	while(1) {

		memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

		FD_SET(fd, &rset);
		FD_SET(fdTCP, &rset);
		select(max, &rset, NULL, NULL, NULL);

		if(FD_ISSET(fd, &rset)) {
			n=recvfrom(fd,buffer,BUFFERSIZE,0,(struct sockaddr*) &addr, &addrlen);
			if(n==-1)
				exit(1);
			parse = strtok(buffer, " \n");

			if ((strcmp(parse, "REG") == 0)){
				validReg(fd, addrlen, n, addr, buffer, parse);
			} else if ((strcmp(parse, "LTP") == 0)){
				topic_list(fd, addrlen, n, addr, buffer, count, topic_counter);
			} else if ((strcmp(parse, "PTP") == 0)){
				topic_propose(fd, addrlen, n, addr, buffer, parse);
			} else if ((strcmp(parse, "LQU") == 0)){
				retrieveStoredData();
				question_list(fd, addrlen, n, addr, buffer, parse);
			}
		} else if (FD_ISSET(fdTCP, &rset)){
			if((newfd=accept(fdTCP,(struct sockaddr*)&addrTCP, &addrlenTCP)) == -1)
				exit(1);

			if ((pid = fork()) < 0) {
                perror("Failed to create new process.");
                exit(1);
            } else if (pid == 0) {
            	printf("%s\n", "child created");
				
				char* ptr = buffer;
				int toRead = 4; //para ler comando
				while(toRead > 0) {
					n = read(newfd, ptr, toRead);
					if(n == -1)
						exit(1);
					toRead -= n;
					ptr += n;
				}

				if ((strcmp(buffer, "GQU ") == 0)){
					memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
					question_get(fdTCP, addrlenTCP, nTCP, buffer, parse, newfd);
				} else if ((strcmp(buffer, "QUS ") == 0)) {
					question_submit(fdTCP, addrlenTCP, nTCP, buffer, parse, newfd);
				} else if ((strcmp(buffer, "ANS ") == 0)) {
					answer_submit(fdTCP, addrlenTCP, nTCP, buffer, parse, newfd);
				}
            }
			close(newfd);
		}
	}

	free(buffer);
	free(parse);
	freeaddrinfo(res);
	close(fd);

	return 0;
}
