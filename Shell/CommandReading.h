#ifndef CommandReading_h
#define CommandReading_h

#include <stdio.h>
#include "structures.h"

int variableSubstitution(char**, int);
int readCurrentWord(char**);
struct job *readCommand(void);

#endif
