#ifndef UDP_H
#define UDP_H 1

void reg(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *parse, char *userID);

void topic_list(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *parse, char **topicList);

void topic_propose(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *parse, char *userID, char *topic);

void question_list(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *parse, char *topic);

#endif