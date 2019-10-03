#ifndef DATA_H
#define DATA_H 1

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
struct topic* topicList;
struct topic* topicListEnd;
int topic_counter;

int retrieveStoredData();

void dataInit();

char* addNewTopic(char *Name, char *Author);

struct topic* getTopic(int topicNumber, char *Name);

void addNewQuestion(int topicNumber, char *topicName, char *title, char *textFilePath, char *imageFilePath);

struct question* getQuestion(struct topic* parentTopic, int topicNumber, char *topicName, char *questionTitle, int questionNumber);

void getTopicList(char *buffer);

#endif