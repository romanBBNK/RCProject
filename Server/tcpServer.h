#ifndef TCPSERVER_H
#define TCPSERVER_H 1

int sizeOfFile(char* name);
int sizeOfFile2(char* name);
void readTokenFromServer(int fd, int n, char *buffer);
void writeTokenToServer(int fd, int n, char *buffer);
void writeTokenToServer2(int fd, int n, char *buffer, int size);
void writeToFile(int fd, int n, char *buffer, char *filePath, int size);
void writeToFile2(int fd, int n, char *buffer, char *filePath, int size);
void writeFromFile(int fd, int n, char *buffer, char *filePath, int size);
void writeFromFile2(int fd, int n, char *buffer, char *filePath, int size);
void question_get(int fd, int addrlen, int n, char *buffer, char *parse, int newfd);
void question_submit(int fd, int addrlen, int n, char *buffer, char *parse, int newfd);
void answer_submit(int fd, int addrlen, int n, char *buffer, char *parse, int newfd);

#endif