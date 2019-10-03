#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include "data.h"
//#define PORT "58000"
#define BUFFERSIZE 128
#define MAXMSGSIZE 2048

int retrieveStoredData(){
    /*
     * Retrieves stored data from the txt files.
     */
    return 0;
}

void dataInit(){
    /*
     * Initializes the necessary data structures. Add as necessary.
     */
}

char* addNewTopic(char *Name, char *Author){
    /*
     * Appends a new topic to the end of the topic list.
     */
    struct topic* newTopic = (struct topic*) malloc(sizeof(struct topic));
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

    return "OK";
}

struct topic* getTopic(int topicNumber, char *Name){
    /*
     * Accepts both topic number and name. If searching by name, give topicNumber = 0; Returns NULL if topic not found.
     */
    struct topic* current;

    if(topicNumber!=0){
        current = topicList;
        while(current!=NULL) {
            if (current->number == topicNumber)
                return current;
            current = current->next;
        }
    } else {
        current = topicList;
        while(current!=NULL) {
            if (strcmp(current->name, Name) == 0)
                return current;
            current = current->next;
        }
    }
    return NULL;
}

void addNewQuestion(int topicNumber, char *topicName, char *title, char *textFilePath, char *imageFilePath){

    struct topic* parentTopic = getTopic(topicNumber, topicName);
    struct question* newQuestion = (struct question*) malloc(sizeof(struct question));

    //Sets necessary initial values
    newQuestion->title = title;
    newQuestion->number = parentTopic->question_counter + 1;
    newQuestion->replies_number = 0;
    newQuestion->textFilePath = textFilePath;
    newQuestion->imageFilePath = imageFilePath;
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
    }
}

struct question* getQuestion(struct topic* parentTopic, int topicNumber, char *topicName, char *questionTitle, int questionNumber){
    /*
     * If parentTopic is specified, gets the question from the parentTopic instead of searching for the topic. If parentTopic is NULL, gets the topic through the same info as
     * getTopic() and then gets the question. Returns NULL if question not found.
     */
    struct question* current;

    if(parentTopic != NULL){
        current = parentTopic->questions;
    } else {
        current = (getTopic(topicNumber, topicName))->questions;
    }
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