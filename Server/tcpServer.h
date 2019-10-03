#ifndef TCPSERVER_H
#define TCPSERVER_H 1

void question_get(int fd, int addrlen, int n, char *buffer, char *parse, int newfd);

void question_submit(int fd, int addrlen, int n, char *buffer, char *parse, int newfd);

void answer_submit(int fd, int addrlen, int n, char *buffer, char *parse, int newfd);

#endif