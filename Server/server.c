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


/*
 * Declaration of necessary structures for storage of the forum contents.
 */
struct answer {
    char *name;
    int number;
    char *textFilePath;
    char *imageFilePath;
    struct answer* next;
};

struct question{
    char *title;
    int number;
    int replies_number;
    char *textFilePath;
    char *imageFilePath;
    struct answer* answers;
    struct question* next;
};

struct topic{
    char *name;
    int number;
    char *author;
    int question_counter;
    struct question* questions;
    struct question* lastQuestion;
    struct topic* next;
};


/*
 * Global variables to be used.
 */
char* port;
struct topic* topicList = NULL;
struct topic* topicListEnd = NULL;
int topic_counter = 0;

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

int retrieveStoredData(){
    /*
     * Retrieves stored data from the txt files.
     */
    return 0;
}

void dataInit(){
    /*
     * Initializes the necessary data structures. Add as necessary.
     */
}

void addNewTopic(char *Name, char *Author){
    /*
     * Appends a new topic to the end of the topic list.
     */
    struct topic* newTopic = (struct topic*) malloc(sizeof(struct topic));
    topic_counter++;
    newTopic->name = Name;
    newTopic->number = topic_counter;
    newTopic->author = Author;
    newTopic->question_counter = 0;
    newTopic->questions = NULL;
    newTopic->lastQuestion = NULL;
    newTopic->next = NULL;
    if(topicList == NULL){
        topicList = newTopic;
        topicListEnd = topicList;
    } else{
        topicListEnd->next = newTopic;
        topicListEnd = newTopic;
    }
}

struct topic* getTopic(int topicNumber, char *Name){
    /*
     * Accepts both topic number and name. If searching by name, give topicNumber = 0; Returns NULL if topic not found.
     */
    struct topic* current;

    if(topicNumber!=0){
        current = topicList;
        while(current!=NULL) {
            if (current->number == topicNumber)
                return current;
            current = current->next;
        }
    } else {
        current = topicList;
        while(current!=NULL) {
            if (strcmp(current->name, Name) == 0)
                return current;
            current = current->next;
        }
    }
    return NULL;
}

void addNewQuestion(int topicNumber, char *topicName, char *title, char *textFilePath, char *imageFilePath){

    struct topic* parentTopic = getTopic(topicNumber, topicName);
    struct question* newQuestion = (struct question*) malloc(sizeof(struct question));

    //Sets necessary initial values
    newQuestion->title = title;
    newQuestion->number = parentTopic->question_counter + 1;
    newQuestion->replies_number = 0;
    newQuestion->textFilePath = textFilePath;
    newQuestion->imageFilePath = imageFilePath;
    newQuestion->answers = NULL;
    newQuestion->next = NULL;

    //Adds to parent topic
    if(parentTopic->question_counter == 0){
        parentTopic->questions = newQuestion;
        parentTopic->lastQuestion = newQuestion;
        parentTopic->question_counter = parentTopic->question_counter +1;
    } else{
        (parentTopic->lastQuestion)->next = newQuestion;
        parentTopic->lastQuestion = newQuestion;
    }
}

struct question* getQuestion(struct topic* parentTopic, int topicNumber, char *topicName, char *questionTitle, int questionNumber){
    /*
     * If parentTopic is specified, gets the question from the parentTopic instead of searching for the topic. If parentTopic is NULL, gets the topic through the same info as
     * getTopic() and then gets the question. Returns NULL if question not found.
     */
    struct question* current;

    if(parentTopic != NULL){
        current = parentTopic->questions;
    } else {
        current = (getTopic(topicNumber, topicName))->questions;
    }
    if(questionNumber!=0){
        while(current!=NULL) {
            if (current->number == questionNumber)
                return current;
            current = current->next;
        }
    } else {
        while(current!=NULL) {
            if (strcmp(current->title, questionTitle) == 0)
                return current;
            current = current->next;
        }
    }
    return NULL;
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