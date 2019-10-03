#ifndef UDPSERVER_H
#define UDPSERVER_H 1

void validReg(int fd, int addrlen, int n, struct sockaddr_in addr, char *buffer, char *parse);

void topic_list(int fd, int addrlen, int n, struct sockaddr_in addr, char *buffer, char count[], int topic_counter);

void topic_propose(int fd, int addrlen, int n, struct sockaddr_in addr, char *buffer, char *parse);

void question_list(int fd, int addrlen, int n, struct sockaddr_in addr, char *buffer, char *parse);

#endif