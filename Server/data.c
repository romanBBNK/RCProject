#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include "data.h"
//#define PORT "58000"
#define BUFFERSIZE 10000
#define MAXMSGSIZE 2048

//Topic related function
struct topic* addNewTopic(char *Name, char *Author){
    //Appends a new topic to the end of the topic list.
    struct topic* newTopic = (struct topic*) malloc(sizeof(struct topic));
    struct topic* current;

    //Checks if the topic list is full or if the title is duplicate. The returned topic will have it's number be 0,
    //which flags it as an error, and it's question_counter will specify the error (2 for duplicate, 1 for full)
    if(topic_counter==99){
        newTopic->number = 0;
        newTopic->question_counter = 1;
        return newTopic;
    }
    if(topicList != NULL){
        current = topicList;
        while(current!=NULL){
            if(strcmp(current->name, Name)==0){
                newTopic->number = 0;
                newTopic->question_counter = 2;
                return newTopic;
            }
            current = current->next;
        }
    }
    topic_counter++;

    newTopic->name = (char *)malloc(strlen(Name)*sizeof(char));
    strcpy(newTopic->name, Name);

    newTopic->number = topic_counter;

    newTopic->author = (char *)malloc(strlen(Author)*sizeof(char));
    strcpy(newTopic->author, Author);

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

    return newTopic;
}
struct topic* getTopic(char *Name){
    /*
     * Accepts both topic number and name. If searching by name, give topicNumber = 0; Returns NULL if topic not found.
     */
    struct topic* current;

    current = topicList;
    while (current != NULL) {
        if (strcmp(current->name, Name) == 0){
            return current;
        }
        current = current->next;
    }
    return NULL;
}
void getTopicList(char *buffer){

    struct topic* current;
    current = topicList;
    while(current!=NULL){
        strcat(buffer, " ");
        strcat(buffer, current->name);
        strcat(buffer, ":");
        strcat(buffer, current->author);
        current = current->next;
    }
}
int saveNewTopic(char *Name, char *Author){

    FILE *topicsFile;
    char *folderPath;
    struct topic* currentTopic;

    //Adds the topic to program memory
    if( (currentTopic = addNewTopic(Name, Author))->number == 0){
        //Topic cannot be added. Returns 1 if full or 2 if duplicate
        return currentTopic->question_counter;
    }

    //Sets the path for the folder and creates it. It will be "./Data/<topicName>
    //With read/write/search permissions for owner and group,
    // and with read/search permissions for others
    folderPath = (char *)malloc(40 * sizeof(char));
    strcpy(folderPath, "./Data/");
    strcat(folderPath, currentTopic->name);
    if( mkdir(folderPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1){
        printf("Error creating topic folder.\n");
        exit(-1);
    }

    //Opens/creates "./Data/topics.txt", and appends topic info
    if((topicsFile = fopen("./Data/topics.txt", "a")) == NULL){
        printf("Error reading topics.txt.\n");
        exit(-1);
    }
    fprintf(topicsFile, "%s\n", Name);
    fprintf(topicsFile, "%s\n", Author);

    //Closes the file and frees the allocated buffer
    fclose(topicsFile);
    free(folderPath);

    return 0;
}


//Question related functions
struct question* addNewQuestion(struct topic* parentTopic, char *Title, char *Author, char *imgExt){

    struct question* newQuestion = (struct question*) malloc(sizeof(struct question));
    struct question* current;

    //Checks if the question list is full or if the question is a duplicate
    //Number=0 means error, replies_number is 2(duplicate) or 1(full)
    if(parentTopic->question_counter == 99){
        newQuestion->number = 0;
        newQuestion->replies_number = 1;
        return newQuestion;
    }
    if(parentTopic->questions != NULL){
        current = parentTopic->questions;
        while(current!=NULL){
            if(strcmp(current->title, Title)==0){
                newQuestion->number = 0;
                newQuestion->replies_number = 2;
                return newQuestion;
            }
            current = current->next;
        }
    }

    //Sets necessary initial values
    newQuestion->title = (char *)malloc(strlen(Title)*sizeof(char));
    strcpy(newQuestion->title, Title);

    newQuestion->author = (char *)malloc(strlen(Author)*sizeof(char));
    strcpy(newQuestion->author, Author);

    newQuestion->number = parentTopic->question_counter + 1;
    newQuestion->replies_number = 0;

    newQuestion->imgExt = (char *)malloc(5*sizeof(char));
    strcpy(newQuestion->imgExt, imgExt);

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
        parentTopic->question_counter = parentTopic->question_counter +1;
    }
    return newQuestion;
}
struct question* getQuestion(char *topicName, char *questionTitle, int questionNumber){
    /*
     * Returns NULL if question not found.
     */
    struct question* current;

    current = (getTopic(topicName))->questions;
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
int getQuestionList(char *buffer, char *topicName){

    struct topic* parentTopic;
    struct question* current;

    parentTopic = getTopic(topicName);

    memset(buffer, '\0', sizeof(char)*BUFFERSIZE);

    char count[2];
    strcat(buffer, "LQR ");

    if(parentTopic == NULL) {
        strcat(buffer, "0");
        return -1;
    }
    sprintf(count, "%d", parentTopic->question_counter);
    strcat(buffer, count);


    char num[2];

    current = parentTopic->questions;
    while(current!=NULL){
        strcat(buffer, " ");
        strcat(buffer, current->title);
        strcat(buffer, ":");
        strcat(buffer, current->author);
        strcat(buffer, ":");
        sprintf(num, "%d", current->number);
        strcat(buffer, num);
        current = current->next;
    }
    return 0;
}
int getLastQuestionList(char *buffer, char *topicName){

    int startQuest;
    struct topic* parentTopic;
    struct question* current;

    parentTopic = getTopic(topicName);

    if( parentTopic->question_counter <=10)
        return getQuestionList(buffer, topicName);

    startQuest = parentTopic->question_counter -9;
    memset(buffer, '\0', sizeof(char)*BUFFERSIZE);
    strcat(buffer, "LQR ");

    if(parentTopic == NULL) {
        strcat(buffer, "0");
        return -1;
    }

    strcat(buffer, "10");

    char num[2];

    current = parentTopic->questions;
    while(current!=NULL){
        if(current->number >= startQuest) {
            strcat(buffer, " ");
            strcat(buffer, current->title);
            strcat(buffer, ":");
            strcat(buffer, current->author);
            strcat(buffer, ":");
            sprintf(num, "%d", current->number);
            strcat(buffer, num);
        }
        current = current->next;
    }
    return 0;
}
int saveNewQuestion(struct topic* parentTopic, char *Title, char *Author, char *imgExt){

    FILE *questionsFile;
    char *folderPath;
    struct question* currentQuestion;

    if(imgExt==NULL){
        //Adds the question to program memory
        if( (currentQuestion = addNewQuestion(parentTopic, Title, Author, "NULL"))->number == 0){
            //Question cannot be added (full or duplicate)
            return currentQuestion->replies_number;
        }
    } else{
        //Adds the question to program memory
        if( (currentQuestion = addNewQuestion(parentTopic, Title, Author, imgExt))->number == 0){
            //Question cannot be added (full or duplicate)
            return currentQuestion->replies_number;
        }
    }

    //Sets the path for the folder and creates it. It will be "./Data/<topicName>/<questionTitle>
    //With read/write/search permissions for owner and group,
    // and with read/search permissions for others
    folderPath = (char *)malloc(40 * sizeof(char));
    strcpy(folderPath, "./Data/");
    strcat(folderPath, parentTopic->name);
    strcat(folderPath, "/");
    strcat(folderPath, currentQuestion->title);
    errno=0;
    if( mkdir(folderPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) !=0 && errno != EEXIST){
        printf("Error creating question folder.\n");
        exit(-1);
    }

    //Opens/creates "./Data/<topicName>/questions.txt", and appends question info
    strcpy(folderPath, "./Data/");
    strcat(folderPath, parentTopic->name);
    strcat(folderPath, "/questions.txt");
    if((questionsFile = fopen(folderPath, "a")) == NULL){
        printf("Error reading questions.txt.\n");
        exit(-1);
    }
    fprintf(questionsFile, "%s\n", Title);
    fprintf(questionsFile, "%s\n", Author);
    fprintf(questionsFile, "%s\n", currentQuestion->imgExt);
    fclose(questionsFile);

    free(folderPath);

    return 0;
}


//Answer related functions
struct answer* addNewAnswer(struct question* parentQuestion, char *Name, char *Author, char *imgExt){

    struct answer* newAnswer = (struct answer*) malloc(sizeof(struct answer));
    struct answer* current;

    //Checks if the answer list is full or if the answer is a duplicate
    if(parentQuestion->replies_number == 99){
        newAnswer->name = NULL;
        newAnswer->number = 1;
        return newAnswer;
    }
    if(parentQuestion->answers != NULL){
        current = parentQuestion->answers;
        while(current!=NULL){
            if(strcmp(current->name, Name)==0){
                newAnswer->name = NULL;
                newAnswer->number = 2;
                return newAnswer;
            }
            current = current->next;
        }
    }

    //Fills the necessary fields
    newAnswer->name = (char *)malloc(strlen(Name)*sizeof(char));
    strcpy(newAnswer->name, Name);

    newAnswer->author = (char *)malloc(strlen(Author)*sizeof(char));
    strcpy(newAnswer->author, Author);

    newAnswer->number = parentQuestion->replies_number + 1;

    newAnswer->imgExt = (char *)malloc(5*sizeof(char));
    strcpy(newAnswer->imgExt, imgExt);

    newAnswer->next = NULL;

    //Adds to parent question
    if(parentQuestion->replies_number == 0){
        parentQuestion->answers = newAnswer;
        parentQuestion->replies_number = parentQuestion->replies_number + 1;
    } else{
        (parentQuestion->answers)->next = newAnswer;
        parentQuestion->replies_number = parentQuestion->replies_number + 1;
    }

    return newAnswer;
}
struct answer* getAnswer(char *parentTopic, char *parentQuestion, int answerNumber){

    struct answer* current;

    current = (getQuestion(parentTopic, parentQuestion, 0))->answers;
    if(answerNumber!=0){
        while(current!=NULL) {
            if (current->number == answerNumber)
                return current;
            current = current->next;
        }
    }
    return NULL;
}
int saveNewAnswer(char *parentTopic, struct question* parentQuestion, char *Author, char *imgExt){

    FILE *answersFile;
    char *folderPath;
    char *answerName;
    char numBuf[3];
    struct answer* currentAnswer;

    //Creates the name for the answer, <question>_XX
    answerName = (char *)malloc(20 * sizeof(char));
    strcpy(answerName, parentQuestion->title);
    if(parentQuestion->replies_number <=8){ strcat(answerName, "_0");
    } else { strcat(answerName, "_"); }
    sprintf(numBuf, "%d", parentQuestion->replies_number + 1);
    strcat(answerName, numBuf);
    
    if(imgExt==NULL) {
        //Adds the answer to program memory
        if ((currentAnswer = addNewAnswer(parentQuestion, answerName, Author, "NULL"))->name == NULL)
            return currentAnswer->number;
    } else {
        //Adds the answer to program memory
        if ((currentAnswer = addNewAnswer(parentQuestion, answerName, Author, imgExt))->name == NULL)
            return currentAnswer->number;
    }

    //Sets the path for the folder and creates it. It will be "./Data/<topicName>/<questionTitle>/<Answer_title>
    //With read/write/search permissions for owner and group,
    // and with read/search permissions for others
    folderPath = (char *)malloc(64 * sizeof(char));
    strcpy(folderPath, "./Data/");
    strcat(folderPath, parentTopic);
    strcat(folderPath, "/");
    strcat(folderPath, parentQuestion->title);
    strcat(folderPath, "/");
    strcat(folderPath, currentAnswer->name);
    errno = 0;
    if( mkdir(folderPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) !=0 && errno != EEXIST){
        printf("Error creating answer folder.\n");
        exit(-1);
    }

    //Opens/creates "./Data/<topicName>/<questionTitle>/answers.txt", and appends answer info
    strcpy(folderPath, "./Data/");
    strcat(folderPath, parentTopic);
    strcat(folderPath, "/");
    strcat(folderPath, parentQuestion->title);
    strcat(folderPath, "/answers.txt");
    if((answersFile = fopen(folderPath, "a")) == NULL){
        printf("Error reading answers.txt.\n");
        exit(-1);
    }
    fprintf(answersFile, "%s\n", currentAnswer->name);
    fprintf(answersFile, "%s\n", Author);
    fprintf(answersFile, "%s\n", currentAnswer->imgExt);
    fclose(answersFile);

    free(answerName);
    free(folderPath);

    return 0;
}
int getLastAnswerNumber(struct question* parentQuestion){

    struct answer* current;

    current = parentQuestion->answers;
    while(current!=NULL){
        if(current->number == parentQuestion->replies_number)
            return current->number;
        current = current->next;
    }
    return -1;
}


//Miscellaneous functions
int retrieveStoredData(){
    /*
     * Retrieves stored data from the txt files.
     */

    //Reads from topics.txt, path is relative for portability: ./Data/topics.txt
    FILE *topicsFile;
    char *readTopicName;
    char *readTopicAuthor;
    struct topic* currentTopic;
    size_t nameSize = 12;
    size_t readName;
    size_t authorSize = 7;
    size_t readAuthor;

    readTopicName = (char *)malloc(12 * sizeof(char));
    readTopicAuthor = (char *)malloc(7 * sizeof(char));

    //Checks if the data folder exists and creates it otherwise
    errno = 0;
    if( mkdir("./Data", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) !=0 && errno != EEXIST){
        printf("Error creating Data folder.\n");
        return -1;
    }

    //Checks if topics.txt exists (and therefore if there is data to retrieve or not)
    if( access("./Data/topics.txt", F_OK) != -1){
        //File exists
        if((topicsFile = fopen("./Data/topics.txt", "r")) == NULL){
            printf("Error reading topics.txt.\n");
            exit(-1);
        }
    } else{
        //Topics.txt doesn't exist (this means it's the first time the server is run and there is no previous
        //Data to load. Returns
        return 0;
    }

    //Reads a topic and acts appropriately
    while((readName = getline(&readTopicName, &nameSize, topicsFile)) != -1){
        readAuthor = getline(&readTopicAuthor, &authorSize, topicsFile);

        //Changes the /n of the string to a null terminator, \0
        readTopicName[readName - 1] = '\0';
        readTopicAuthor[readAuthor - 1] = '\0';

        //Saves the topic and calls the function to pursue questions related to the topic
        currentTopic = addNewTopic(readTopicName, readTopicAuthor);
        if(currentTopic->number != 0){
            //Completely new topic
            retrieveStoredQuestions(currentTopic);
        } else {
            if(currentTopic->question_counter==2){
                //Duplicate addition of topic
                retrieveStoredQuestions( getTopic(readTopicName) );
            } else {
                //Error adding topic because topiclist is full
                printf("Error adding topic to memory\n");
                exit(-1);
            }
        }

        //Resets the buffers used
        memset(readTopicName, 0, 12);
        memset(readTopicAuthor, 0, 7);
    }

    //Frees allocated memory and closes the file
    free(readTopicAuthor);
    free(readTopicName);
    fclose(topicsFile);

    return 0;
}
int retrieveStoredQuestions(struct topic* currentTopic){

    //Reads from questions.txt, path is relative for portability: ./Data/<TopicName>/questions.txt
    FILE *questionsFile;
    char *questionsFilePath;
    char *readQstTitle;
    char *readQstAuthor;
    char *readImgPath;
    struct question* currentQuestion;
    size_t titleSize = 12;
    size_t readTitle;
    size_t authorSize = 7;
    size_t readAuthor;
    size_t pathSize = 16;
    size_t readImage;

    questionsFilePath = (char *)malloc(40 * sizeof(char));
    readQstTitle = (char *)malloc(12 * sizeof(char));
    readQstAuthor = (char *)malloc(7 * sizeof(char));
    readImgPath = (char *)malloc(16 * sizeof(char));

    //Sets the filepath for the topic folder
    strcpy(questionsFilePath, "./Data/");
    strcat(questionsFilePath, currentTopic->name);
    strcat(questionsFilePath, "/questions.txt");

    //Opens questions.txt

    if( access(questionsFilePath, F_OK) != -1){
        //File exists
        if((questionsFile = fopen(questionsFilePath, "r")) == NULL){
            printf("Error reading questions.txt.\n");
            exit(-1);
        }
    } else{
        //questions.txt doesn't exist (this means it's the first time the server is run and there is no previous
        //Data to load. Returns.
        return 0;
    }

    //Reads a question and acts appropriately
    while((readTitle = getline(&readQstTitle, &titleSize, questionsFile)) != -1){
        readAuthor = getline(&readQstAuthor, &authorSize, questionsFile);
        readImage = getline(&readImgPath, &pathSize, questionsFile);

        //Changes the /n of the string to a null terminator, \0
        readQstTitle[readTitle - 1] = '\0';
        readQstAuthor[readAuthor - 1] = '\0';
        readImgPath[readImage - 1] = '\0';

        //Saves the topic and calls the function to pursue answers related to the question
        currentQuestion = addNewQuestion(currentTopic, readQstTitle, readQstAuthor, readImgPath);
        if(currentQuestion->number != 0){
            //Completely new question
            retrieveStoredAnswers(currentTopic, currentQuestion);
        } else {
            if(currentQuestion->replies_number==2){
                //Duplicate addition of question
                retrieveStoredAnswers(currentTopic, getQuestion(currentTopic->name, readQstTitle, 0) );
            } else {
                //Error adding question because question list is full
                printf("Error adding question to memory\n");
                exit(-1);
            }
        }

        //Resets the buffers used
        memset(readQstTitle, 0, 12);
        memset(readQstAuthor, 0, 7);
        memset(readImgPath, 0, 16);
    }

    //Cleans up temporarily allocated memory and closes the file
    free(readQstTitle);
    free(readQstAuthor);
    free(questionsFilePath);
    free(readImgPath);
    fclose(questionsFile);

    return 0;
}
int retrieveStoredAnswers(struct topic* currentTopic, struct question* currentQuestion){

    //Reads from answers.txt, path is relative for portability: ./Data/<TopicName>/<QuestionName>/answers.txt
    FILE *answersFile;
    char *answersFilePath;
    char *readAnsName;
    char *readAnsAuthor;
    char *readImgPath;
    struct answer* currentAnswer;
    size_t nameSize = 15;
    size_t readName;
    size_t authorSize = 7;
    size_t readAuthor;
    size_t pathSize = 16;
    size_t readImage;

    answersFilePath = (char *)malloc(64 * sizeof(char));
    readAnsName = (char *)malloc(15 * sizeof(char));
    readAnsAuthor = (char *)malloc(7 * sizeof(char));
    readImgPath = (char *)malloc(16 * sizeof(char));

    //Sets the filepath for the question folder
    strcpy(answersFilePath, "./Data/");
    strcat(answersFilePath, currentTopic->name);
    strcat(answersFilePath, "/");
    strcat(answersFilePath, currentQuestion->title);
    strcat(answersFilePath, "/answers.txt");

    //Opens answers.txt
    if( access(answersFilePath, F_OK) != -1){
        //File exists
        if((answersFile = fopen(answersFilePath, "r")) == NULL){
            printf("Error reading answers.txt.\n");
            exit(-1);
        }
    } else{
        //answers.txt doesn't exist (this means it's the first time the server is run and there is no previous
        //Data to load. Returns.
        return 0;
    }

    //Reads an answer and acts appropriately
    while((readName = getline(&readAnsName, &nameSize, answersFile)) != -1){
        readAuthor = getline(&readAnsAuthor, &authorSize, answersFile);
        readImage = getline(&readImgPath, &pathSize, answersFile);

        //Changes the /n of the string to a null terminator, \0
        readAnsName[readName - 1] = '\0';
        readAnsAuthor[readAuthor - 1] = '\0';
        readImgPath[readImage - 1] = '\0';


        //Saves the answer
        currentAnswer = addNewAnswer(currentQuestion, readAnsName, readAnsAuthor, readImgPath);
        if(currentAnswer->name == NULL){
            //Error adding
            if(currentAnswer->number != 2){
                //Error adding question because question list is full
                printf("Error adding question to memory\n");
                exit(-1);
            }
        }

        //Resets the buffers used
        memset(readAnsName, 0, 12);
        memset(readAnsAuthor, 0, 7);
        memset(readImgPath, 0, 16);
    }

    //Cleans up temporarily allocated memory and closes the file
    free(readAnsName);
    free(readAnsAuthor);
    free(answersFilePath);
    free(readImgPath);
    fclose(answersFile);

    return 0;
}
int generateFilePath(char* topic, char* question, char* answer, char *imgExt, char *targetPath){

    //Sets base dir and topic
    strcpy(targetPath, "./Data/");
    strcat(targetPath, topic);

    //Creates folder if nonexistent
    errno = 0;
    if( mkdir(targetPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) !=0 && errno != EEXIST){
        printf("Error creating topic folder.\n");
        return -1;
    }

    //Checks if we want a path to a question
    if(question==NULL)
        return 0;
    else{
        strcat(targetPath, "/");
        strcat(targetPath, question);
        errno = 0;
        if( mkdir(targetPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) !=0 && errno != EEXIST){
            printf("Error creating question folder.\n");
            return -1;
        }
    }
    //Current path: ./Data/topic/question

    //Checks if we want a path to an answer
    if(answer==NULL){
        strcat(targetPath, "/");
        strcat(targetPath, question);
        strcat(targetPath, ".");
        //Checks if the path is for an image or text
        if(imgExt==NULL){
            //Current path: ./Data/topic/question/question.txt
            strcat(targetPath, "txt");
            return 0;
        } else{
            //Current path: ./Data/topic/question/question.imgExt
            strcat(targetPath, imgExt);
            return 0;
        }
    } else{
        strcat(targetPath, "/");
        strcat(targetPath, answer);
        errno = 0;
        //Current path: ./Data/topic/question/answer
        if( mkdir(targetPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) !=0 && errno != EEXIST){
            printf("Error creating answer folder.\n");
            return -1;
        }
        strcat(targetPath, "/");
        strcat(targetPath, answer);
        strcat(targetPath, ".");
        //Checks if the path is for an image or text
        if(imgExt == NULL){
            //Current path: ./Data/topic/question/answer/answer.txt
            strcat(targetPath, "txt");
            return 0;
        } else{
            //Current path: ./Data/topic/question/answer/answer.imgExt
            strcat(targetPath, imgExt);
            return 0;
        }
    }
}
