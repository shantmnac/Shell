#include <stdio.h>
#include <stdlib.h>

#include "structures.h"
#include "SideFunctions.h"
#include "CommandReading.h"

struct environmentVariables globalVariables;

int main(int argc, const char * argv[]) {
    struct job *tmp;
    initializationOfglobalVariables(argc, argv);
   
    while (1) {
        printf("Shant$ ");
        tmp = readCommand();
    }
    return 0;
}
