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
    char *imageFilePath;
    struct answer* next;
};
struct question{
    char *title;
    char *author;
    int number;
    int replies_number;
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

//Topic related functions
struct topic* addNewTopic(char *Name, char *Author);
struct topic* getTopic(char *Name);
void getTopicList(char *buffer);

//Question related functions
struct question* addNewQuestion(struct topic* parentTopic, char *Title, char *Author, char *imageFilePath);
struct question* getQuestion(struct topic* parentTopic, char *topicName, char *questionTitle, int questionNumber);
int getQuestionList(char *buffer, char *topicName);


//Answer related functions
//TODO Add answer functions for adding and getting answers
struct answer* addNewAnswer(struct question* parentQuestion, char *Name, char *Author, char *imageFilePath);
struct answer* getAnswer();
int getAnswerList();

//Miscellaneous functions
int retrieveStoredData();
int retrieveStoredQuestions(struct topic* currentTopic);
int retrieveStoredAnswers(struct topic* currentTopic ,struct question* currentQuestion);
void dataInit();

#endif
