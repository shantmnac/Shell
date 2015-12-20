#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "CommandReading.h"
#include "SideFunctions.h"
#include "structures.h"

extern struct environmentVariables globalVariables;
extern int jobsCounter;

struct programm initializationOfNewProgramm(void){
    struct programm tmpProgramm;
    
    tmpProgramm.arguments = NULL;
    tmpProgramm.inputFile = tmpProgramm.outputFile = NULL;
    tmpProgramm.outputType = 1;
    tmpProgramm.name = NULL;
    
    return tmpProgramm;
}

struct job initializationOfNewJob(void){
    struct job tmpJob;
    
    tmpJob.background = 0;
    tmpJob.numberOfPrograms = 0;
    tmpJob.programs = (struct programm*)malloc(sizeof(struct programm));
    tmpJob.programs[0] = initializationOfNewProgramm();

    return tmpJob;
}

char *strCopy(char *inputStr, int inputStrSize){
    char *tmpStr = (char*)malloc(inputStrSize + 1);
    int i;
    
    for (i = 0; i < inputStrSize; i++) {
        tmpStr[i] = inputStr[i];
    }
    
    return tmpStr;
}

int variableSubstitution(char **currentWord, int currentWordSize){
    char tempChar, *variableName = NULL, *replacementString = NULL;
    int variableNameSize = 0, numOfArgv = 0, isVariableCorrect = 0, j = 0;
    
    if ((tempChar = getchar()) == '{') {
        while ((tempChar = getchar()) != '}') {
            if (tempChar == '\n') {
                fprintf(stderr, "variableSubstitution: expected bracket\n");
                if (variableName != NULL) {
                    free(variableName);
                }
                return -1;
            }
            if (!variableNameSize) {
                variableName = (char*)malloc(1);
            }
            else{
                variableName = (char*)realloc(variableName, variableNameSize + 1);
            }
            if (variableName == NULL) {
                memmoryAllarm("readCurrentWord");
            }
            variableName[variableNameSize] = tolower(tempChar);
            variableNameSize++;
        }
        
        variableName = (char*)realloc(variableName, variableNameSize + 1);
        if (variableName == NULL) {
            memmoryAllarm("readCurrentWord");
        }
        variableName[variableNameSize] = '\0';


        if (!strcmp(variableName, "user")) {
            replacementString = globalVariables.username;
            isVariableCorrect = 1;
        }
        if (!strcmp(variableName, "home")) {
            replacementString = globalVariables.homeDirectory;
            isVariableCorrect = 1;
        }
        if (!strcmp(variableName, "shell")) {
            replacementString = globalVariables.shellDirectory;
            isVariableCorrect = 1;
        }
        if (!strcmp(variableName, "uid")) {
            replacementString = itoa(globalVariables.UID);
            isVariableCorrect = 1;
        }
        if (!strcmp(variableName, "pwd")) {
            replacementString = globalVariables.currentDirectory;
            isVariableCorrect = 1;
        }
        if (!strcmp(variableName, "pid")) {
            replacementString = itoa(globalVariables.PID);
            isVariableCorrect = 1;
        }
        
        if (!isVariableCorrect) {
            fprintf(stderr, "variableSubstitution: incorrect variable\n");
            if (variableName != NULL) {
                free(variableName);
            }
            return -1;
        }
    }
    else{
        switch (tempChar) {
                
            case '?':
                replacementString = itoa(globalVariables.statusOfLastProcess);
                break;
            
            case '#':
                replacementString = itoa(globalVariables.argc);
                break;
            
            default:
                if(isdigit(tempChar)){
                    numOfArgv = atoi(&tempChar);
                    while ((tempChar = getchar()) != ' ') {
                        if (tempChar == '\n') {
                            
                        }
                        else{
                            if (isdigit(tempChar)) {
                                numOfArgv = numOfArgv * 10 + atoi(&tempChar);
                            }
                            else{
                                fprintf(stderr, "variableSubstitution: expected digit\n");
                                return -1;
                            }
                        }
                    }
                    
                    if (numOfArgv + 1 > globalVariables.argc) {
                        fprintf(stderr, "variableSubstitution: incorrect variable\n");
                        return -1;
                    }
                    else{
                        replacementString = globalVariables.argv[numOfArgv];
                    }
                }
                else {
                    fprintf(stderr, "variableSubstitution: expected digit\n");
                    return -1;
                }
                
                break;
        }
    }
    
    while (replacementString[j] != '\0') {
        (*currentWord) = (char*)realloc((*currentWord), currentWordSize + 1);
        if ((*currentWord) == NULL) {
            memmoryAllarm("readCurrentWord");
        }
        (*currentWord)[currentWordSize] = replacementString[j];
        currentWordSize++;
        j++;
    }
    
    return currentWordSize;
}

int readCurrentWord(char **currentWord){
    char tempChar = '!';
    int currentWordSize = 0, quotes = 0;
    
    switch (tempChar = getchar()) {
        case '\'':
            quotes = 2;
            break;
        
        case '"':
            quotes = 1;
            break;
            
        default:
            break;
    }
    
    ungetc(tempChar, stdin);
    
    switch (quotes) {
        case 0:{
            while ((tempChar = getchar()) != ' ') {
                if ((tempChar == '>') || (tempChar == '<') || (tempChar == ';') || (tempChar == '|') || (tempChar == '\n') || (tempChar == '&')) {
                    ungetc(tempChar, stdin);
                    break;
                }
                else{
                    if (tempChar == '$') {
                        currentWordSize = variableSubstitution(currentWord, currentWordSize);
                        if (currentWordSize == -1) {
                            if ((*currentWord) != NULL) {
                                free((*currentWord));
                                *currentWord = NULL;
                            }
                            return -1;
                        }
                        continue;
                    }
                    
                    if (tempChar == '\\') {
                        tempChar = getchar();
                        
                        switch (tempChar) {
                                
                            case 'r':
                                tempChar = '\r';
                                break;
                                
                            case 't':
                                tempChar = '\t';
                                break;
                                
                            case 'n':
                                tempChar = '\n';
                                break;
                                
                            case '\\':
                                tempChar = '\\';
                                break;
                                
                            case 'b':
                                tempChar = '\b';
                                break;
                                
                            case '"':
                                tempChar = '"';
                                break;
                                
                            case '\'':
                                tempChar = '\'';
                                break;
                                
                            case '$':
                                tempChar = '$';
                                break;
                                
                            case '\n':
                                ungetc(tempChar, stdin);
                                (*currentWord) = (char*)realloc((*currentWord), currentWordSize + 1);
                                if ((*currentWord) == NULL) {
                                    memmoryAllarm("readCurrentWord");
                                }
                                (*currentWord)[currentWordSize] = '\0';
                                
                                return currentWordSize;
                                
                            default:
                                break;
                        }
                    }
                    
                    (*currentWord) = (char*)realloc((*currentWord), currentWordSize + 1);
                    if ((*currentWord) == NULL) {
                        memmoryAllarm("readCurrentWord");
                    }
                    ((*currentWord))[currentWordSize] = tempChar;
                    currentWordSize++;
                }
            }
            break;
        }
            
        case 1:{
            tempChar = getchar();
            (*currentWord) = (char*)realloc((*currentWord), currentWordSize + 1);
            if (currentWord == NULL) {
                memmoryAllarm("readCurrentWord");
            }
            (*currentWord)[currentWordSize] = tempChar;
            currentWordSize++;
            
            while ((tempChar = getchar()) != '"') {
                if (tempChar == '\n') {
                    fprintf(stderr, "readCurrentWord: expected quotes\n");
                    if ((*currentWord) != NULL) {
                        free((*currentWord));
                        *currentWord = NULL;
                    }
                    return -1;
                }
                else{
                    if (tempChar == '$') {
                        currentWordSize = variableSubstitution(currentWord, currentWordSize);
                        if (currentWordSize == -1) {
                            if ((*currentWord) != NULL) {
                                free((*currentWord));
                                *currentWord = NULL;
                            }
                            return -1;
                        }
                    }
                    
                    if (tempChar == '\\') {
                        tempChar = getchar();
                        
                        switch (tempChar) {
                                
                            case 'r':
                                tempChar = '\r';
                                break;
                                
                            case 't':
                                tempChar = '\t';
                                break;
                                
                            case 'n':
                                tempChar = '\n';
                                break;
                                
                            case '\\':
                                tempChar = '\\';
                                break;
                                
                            case 'b':
                                tempChar = '\b';
                                break;
                                
                            case '"':
                                tempChar = '"';
                                break;
                                
                            case '$':
                                tempChar = '$';
                                break;
                                
                            case '\n':
                                fprintf(stderr, "readCurrentWord: expected quotes\n");
                                if ((*currentWord) != NULL) {
                                    free((*currentWord));
                                    *currentWord = NULL;
                                }
                                return -1;
                                
                            default:
                                break;
                        }
                    }
                    
                    (*currentWord) = (char*)realloc((*currentWord), currentWordSize + 1);
                    if ((*currentWord) == NULL) {
                        memmoryAllarm("readCurrentWord");
                    }
                    (*currentWord)[currentWordSize] = tempChar;
                    currentWordSize++;
                }
            }
            
            (*currentWord) = (char*)realloc((*currentWord), currentWordSize + 1);
            if ((*currentWord) == NULL) {
                memmoryAllarm("readCurrentWord");
            }
            (*currentWord)[currentWordSize] = tempChar;
            currentWordSize++;
            break;
        }
        
        case '2':{
            tempChar = getchar();
            (*currentWord) = (char*)realloc((*currentWord), currentWordSize + 1);
            if ((*currentWord) == NULL) {
                memmoryAllarm("readCurrentWord");
            }
            (*currentWord)[currentWordSize] = tempChar;
            currentWordSize++;
            
            while ((tempChar = getchar()) != '\'') {
                if (tempChar == '\n') {
                    fprintf(stderr, "readCurrentWord: expected quotes\n");
                    if ((*currentWord) != NULL) {
                        free((*currentWord));
                        *currentWord = NULL;
                    }
                    return -1;
                }
                else{
                    if (tempChar == '\\') {
                        tempChar = getchar();
                        
                        switch (tempChar) {
                                
                            case 'r':
                                tempChar = '\r';
                                break;
                                
                            case 't':
                                tempChar = '\t';
                                break;
                                
                            case 'n':
                                tempChar = '\n';
                                break;
                                
                            case '\\':
                                tempChar = '\\';
                                break;
                                
                            case 'b':
                                tempChar = '\b';
                                break;
                                
                            case '\'':
                                tempChar = '\'';
                                break;
                                
                            case '\n':
                                fprintf(stderr, "readCurrentWord: expected quotes\n");
                                if ((*currentWord) != NULL) {
                                    free((*currentWord));
                                    *currentWord = NULL;
                                }
                                return -1;
                                
                            default:
                                break;
                        }
                    }
                    
                    (*currentWord) = (char*)realloc((*currentWord), currentWordSize + 1);
                    if ((*currentWord) == NULL) {
                        memmoryAllarm("readCurrentWord");
                    }
                    (*currentWord)[currentWordSize] = tempChar;
                    currentWordSize++;
                }
            }
            
            (*currentWord) = (char*)realloc((*currentWord), currentWordSize + 1);
            if ((*currentWord) == NULL) {
                memmoryAllarm("readCurrentWord");
            }
            (*currentWord)[currentWordSize] = tempChar;
            currentWordSize++;
            break;
        }
            
        default:
            break;
    }
    if (currentWordSize) {
        (*currentWord) = (char*)realloc((*currentWord), currentWordSize + 1);
        if ((*currentWord) == NULL) {
            memmoryAllarm("readCurrentWord");
        }
        (*currentWord)[currentWordSize] = '\0';
    }
    
    return currentWordSize;
}

struct job *readCommand(void){///////////////////// написать обработку -1 от readCurrentWord
    char currentChar = '!', (*currentWord) = NULL;
    int programsCounter = 0, argumentsCounter = 0, currentWordSize = 0;
    struct job *currentJobs = NULL;
    
    while (1) {
        if (!jobsCounter) {
            currentJobs = (struct job*)malloc(sizeof(struct job));
        }
        else{
            currentJobs = (struct job*)realloc(currentJobs, (jobsCounter + 1) * sizeof(struct job));
        }
        currentJobs[jobsCounter] = initializationOfNewJob();
        while (1) {
            while ((currentChar = getchar()) == ' '){}
            ungetc(currentChar, stdin);
            
            if(!(currentWordSize = readCurrentWord(&currentWord))){
                
                if (currentWordSize == -1) {
                    
                }
                
                currentChar = getchar();
                if ((currentChar == ';') || (currentChar == '|')){
                    continue;
                }
                if ((currentChar == '>') || (currentChar == '<') || (currentChar == '&')) {
                    while (1) {
                        currentChar = getchar();
                        if ((currentChar == ';') || (currentChar == '|')){
                            break;
                        }
                        else{
                            if (currentChar == '\n') {
                                if (!jobsCounter) {
                                    free(currentJobs[jobsCounter].programs);
                                    free(currentJobs);
                                }
                                else{
                                    if (!programsCounter) {
                                        free(currentJobs[jobsCounter].programs);
                                        jobsCounter--;
                                        currentJobs = (struct job*)realloc(currentJobs, (jobsCounter + 1) * sizeof(struct job));
                                    }
                                }
                                return currentJobs;
                            }
                        }
                    }
                    continue;
                }
                if (currentChar == '\n') {
                    return currentJobs;
                }
            }
            else{
                if (programsCounter) {
                    currentJobs[jobsCounter].programs = (struct programm*)realloc(currentJobs[jobsCounter].programs, sizeof(struct programm) * (programsCounter + 1));
                    currentJobs[jobsCounter].programs[programsCounter] = initializationOfNewProgramm();
                }
                currentJobs[jobsCounter].numberOfPrograms++;
                currentJobs[jobsCounter].programs[programsCounter].name = (char*)malloc(currentWordSize + 1);
                currentJobs[jobsCounter].programs[programsCounter].name = strCopy(currentWord, currentWordSize);
                free(currentWord);
                currentWord = NULL;
                currentWordSize = 0;
            }
            
            while (1) {
                while ((currentChar = getchar()) == ' '){}
                ungetc(currentChar, stdin);
                
                if(!(currentWordSize = readCurrentWord(&currentWord))){
                    currentChar = getchar();
                    if (currentChar == ';'){
                        break;
                    }
                    if (currentChar == '|') {
                        break;
                    }
                    if (currentChar == '&') {
                        currentChar = getchar();
                        if ((currentChar == ';') || ((currentChar == '\n'))){
                            currentJobs[jobsCounter].background = 1;
                            ungetc(currentChar, stdin);
                        }
                        else{
                            ungetc(currentChar, stdin);
                        }
                        continue;
                    }
                    if (currentChar == '>') {
                        if ((currentChar = getchar()) == '>') {
                            while ((currentChar = getchar()) == ' '){}
                            ungetc(currentChar, stdin);
                            if ((currentWordSize = readCurrentWord(&currentWord))){
                                currentJobs[jobsCounter].programs[programsCounter].outputFile = (char*)malloc(currentWordSize + 1);
                                currentJobs[jobsCounter].programs[programsCounter].outputFile = strCopy(currentWord, currentWordSize);
                                currentJobs[jobsCounter].programs[programsCounter].outputType = 2;
                                free(currentWord);
                                currentWord = NULL;
                                currentWordSize = 0;
                            }
                        }
                        else{
                            ungetc(currentChar, stdin);
                            while ((currentChar = getchar()) == ' '){}
                            ungetc(currentChar, stdin);
                            if ((currentWordSize = readCurrentWord(&currentWord))){
                                currentJobs[jobsCounter].programs[programsCounter].outputFile = (char*)malloc(currentWordSize + 1);
                                currentJobs[jobsCounter].programs[programsCounter].outputFile = strCopy(currentWord, currentWordSize);
                                free(currentWord);
                                currentWord = NULL;
                                currentWordSize = 0;
                            }
                        }
                    }
                    if (currentChar == '<') {
                        while ((currentChar = getchar()) == ' '){}
                        ungetc(currentChar, stdin);
                        if ((currentWordSize = readCurrentWord(&currentWord))){
                            currentJobs[jobsCounter].programs[programsCounter].inputFile = (char*)malloc(currentWordSize + 1);
                            currentJobs[jobsCounter].programs[programsCounter].inputFile = strCopy(currentWord, currentWordSize);
                            free(currentWord);
                            currentWord = NULL;
                            currentWordSize = 0;
                        }
                    }
                    if (currentChar == '\n') {
                        return currentJobs;
                    }
                }
                else{
                    if (!argumentsCounter) {
                        currentJobs[jobsCounter].programs[programsCounter].numberOfArguments = 0;
                        currentJobs[jobsCounter].programs[programsCounter].arguments = (char**)malloc(sizeof(char*));
                    }
                    else{
                        currentJobs[jobsCounter].programs[programsCounter].arguments = (char**)realloc(currentJobs[jobsCounter].programs[programsCounter].arguments, sizeof(char*) * (argumentsCounter + 1));
                    }
                    currentJobs[jobsCounter].programs[programsCounter].arguments[argumentsCounter] = (char*)malloc(currentWordSize + 1);
                    currentJobs[jobsCounter].programs[programsCounter].arguments[argumentsCounter] = strCopy(currentWord, currentWordSize);
                    currentJobs[jobsCounter].programs[programsCounter].numberOfArguments++;
                    free(currentWord);
                    currentWord = NULL;
                    currentWordSize = 0;
                    argumentsCounter++;
                    continue;
                }
            }
            
            if (currentChar == ';'){
                break;
            }
            if (currentChar == '|') {
                argumentsCounter = 0;
                programsCounter++;
                continue;
            }
        }
        argumentsCounter = 0;
        programsCounter = 0;
        jobsCounter++;
    }
    
    return 0;
}