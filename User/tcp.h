#ifndef TCP_H
#define TCP_H 1

void connectTCP(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr);

void question_get(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *topic, char *question);

void question_submit(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *parse, char *userID, char *topic, char *question, char *textFile, char *imageFile);

void answer_submit(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *parse, char *userID, char *topic, char *question, char *textFile, char *imageFile);

#endif
