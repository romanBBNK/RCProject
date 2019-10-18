#ifndef TCP_H
#define TCP_H 1

int sizeOfFile(char* name);
int sizeOfFile2(char* name);
void readTokenFromServer(int fd, int n, char *buffer);
void writeTokenToServer(int fd, int n, char *buffer);
void writeTokenToServer2(int fd, int n, char *buffer, int size);

void writeToFile(int fd, int n, char *buffer, char *filePath, int size);
void writeToFile2(int fd, int n, char *buffer, char *filePath, int size);
void writeFromFile(int fd, int n, char *buffer, char *filePath, int size);
void writeFromFile2(int fd, int n, char *buffer, char *filePath, int size);
int generateFilePath(char* topic, char* question, char* answer, char *imgExt, char *targetPath);
void question_get(int newfd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *parse, char *topic, char *question);
void question_submit(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *parse, char *userID, char *topic, char *question, char *textFile, char *imageFile);
void answer_submit(int fd, int addrlen, int n, struct addrinfo *res, struct sockaddr_in addr, char *buffer, char *parse, char *userID, char *topic, char *question, char *textFile, char *imageFile);

#endif
