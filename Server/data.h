#ifndef DATA_H
#define DATA_H 1


char* port;
struct topic* topicList;
struct topic* topicListEnd;
int topic_counter;


/*
 * Declaration of necessary structures for storage of the forum contents.
 */
struct answer {
    char *name;
    char *author;
    int number;
    char *imgExt;
    struct answer* next;
};
struct question{
    char *title;
    char *author;
    int number;
    int replies_number;
    char *imgExt;
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

//Topic related functions
struct topic* addNewTopic(char *Name, char *Author);
struct topic* getTopic(char *Name);
void getTopicList(char *buffer);
int saveNewTopic(char *Name, char *Author);

//Question related functions
struct question* addNewQuestion(struct topic* parentTopic, char *Title, char *Author, char *imgExt);
struct question* getQuestion(char *topicName, char *questionTitle, int questionNumber);
int getQuestionList(char *buffer, char *topicName);
int getLastQuestionList(char *buffer, char *topicName);
int saveNewQuestion(struct topic* parentTopic, char *Title, char *Author, char *imgExt);


//Answer related functions
struct answer* addNewAnswer(struct question* parentQuestion, char *Name, char *Author, char *imgExt);
struct answer* getAnswer(char *parentTopic, char *parentQuestion, int answerNumber);
int saveNewAnswer(char *parentTopic, struct question* parentQuestion, char *Author, char *imgExt);
int getLastAnswerNumber(struct question* parentQuestion);

//data storage/retrieval functions
int retrieveStoredData();
int retrieveStoredQuestions(struct topic* currentTopic);
int retrieveStoredAnswers(struct topic* currentTopic ,struct question* currentQuestion);
int generateFilePath(char* topic, char* question, char* answer, char *imgExt, char *targetPath);


#endif
