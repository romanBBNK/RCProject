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

void validReg(int fd, int addrlen, int n, struct sockaddr_in addr, char buffer[]) {
	n=recvfrom(fd,buffer,128,0,(struct sockaddr*) &addr, &addrlen);
	if(n==-1)
		exit(1);
	write(1,"REG ",4); write(1,buffer,n);
	write(1, "\n", 1);
	if(strcmp(buffer, "89484") == 0) {
		n=sendto(fd,"OK",2,0,(struct sockaddr*) &addr, addrlen);
	}
	else {
		n=sendto(fd,"NOK",3,0,(struct sockaddr*) &addr, addrlen);
	}
	if(n==-1)
		exit(1);
}

void topic_propose(int fd, int addrlen, int n, struct sockaddr_in addr, char buffer[]) {
	write(1,"PTP ",4);
	
	n=recvfrom(fd,buffer,128,0,(struct sockaddr*) &addr, &addrlen);
	if(n==-1)
		exit(1);
	write(1, buffer, n);
	write(1, " ", 1);

	n=recvfrom(fd,buffer,128,0,(struct sockaddr*) &addr, &addrlen);
	if(n==-1)
		exit(1);
	write(1, buffer, n);
	write(1, "\n", 1);

	n=sendto(fd,"OK",2,0,(struct sockaddr*) &addr, addrlen);
}

int main(int argc, char *argv[]){

	int fd, addrlen, n;
	struct addrinfo hints, *res;
	struct sockaddr_in addr;
	char buffer[128];

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

	while(1) {
		n=recvfrom(fd,buffer,128,0,(struct sockaddr*) &addr, &addrlen);
		if(n==-1)
			exit(1);

		if ((strcmp(buffer, "reg") == 0)){
			validReg(fd, addrlen, n, addr, buffer);
		} else if ((strcmp(buffer, "tl") == 0)){
			write(1,"LTP",3);
			write(1, "\n", 1);
		} else if ((strcmp(buffer, "tp") == 0)){
			topic_propose(fd, addrlen, n, addr, buffer);
		}
	}

	freeaddrinfo(res);
	close(fd);

	return 0;
}