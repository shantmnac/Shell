#ifndef CommandReading_h
#define CommandReading_h

#include <stdio.h>
#include "structures.h"

struct programm initializationOfNewProgramm(void);
struct job initializationOfNewJob(void);
char *strCopy(char*, int);
int variableSubstitution(char**, int);
int readCurrentWord(char**);
struct job *readCommand(void);

#endif
