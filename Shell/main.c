#include <stdio.h>
#include <stdlib.h>

#include "structures.h"
#include "SideFunctions.h"
#include "CommandReading.h"

struct environmentVariables globalVariables;
int jobsCounter = 0;

int main(int argc, char * argv[]) {
    struct job *tmp;
    initializationOfGlobalVariables(argc, argv);
   
    while (1) {
        printf("%s$ ", globalVariables.username);
        tmp = readCommand();
        //printf("%s %s %s", tmp[0].programs[0].name, tmp[0].programs[0].arguments[0], tmp[0].programs[0].arguments[1]);
    }
    return 0;
}
