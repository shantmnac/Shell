#include <stdio.h>
#include <stdlib.h>

#include "externalCommands.h"
#include "structures.h"
#include "SideFunctions.h"
#include "CommandReading.h"

struct environmentVariables globalVariables;
int jobsCounter = 0;

int main(int argc, char *argv[]) {
    /*struct job *tmp;
    initializationOfGlobalVariables(argc, argv);
   
    while (1) {
        printf("\x1b[1;31m%s$\x1b[1;31m ", globalVariables.username);
        tmp = readCommand();
    }*/
    
    msed("$", "RRR");
    return 0;
}
