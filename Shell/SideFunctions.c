#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>

#include "structures.h"
#include "SideFunctions.h"

extern struct environmentVariables globalVariables;

void memmoryAllarm(char *callerFunction){
    fprintf(stderr, "%s: memmory allocation issue\n", callerFunction);
}

int initializationOfGlobalVariables(int argc, char *argv[]){
    int i;
    struct passwd *pw = getpwuid(getuid());
    
    globalVariables.argv = (char**)malloc(argc * sizeof(char*));
    
    for (i = 0; i < argc; i++) {
        globalVariables.argv[i] = argv[i];
    }
    
    globalVariables.homeDirectory = pw->pw_dir;
    globalVariables.PID = (int)getpid();
    globalVariables.UID = (int)getuid();
    globalVariables.argc = argc;
    globalVariables.statusOfLastProcess = 0;
    globalVariables.shellDirectory = getcwd(globalVariables.shellDirectory, 0);
    globalVariables.currentDirectory = getcwd(globalVariables.currentDirectory, 0);
    globalVariables.username = getlogin();
    return 0;
}

char *itoa (int n) {
    char *t = NULL;
    int z = 0, j = 0;
    
    if (n < 0) {
        z=1;
        n=-n;
    }
    
    do{
        t = (char*)realloc(t, j + 1);
        t[j] = n%10 + '0';
        j++;
    }while (n/=10);
    
    if (z){
        t = (char*)realloc(t, j + 1);
        t[j] = '-';
        j++;
    }
    
    t = (char*)realloc(t, j + 1);
    t[j] = '\0';
    return reverse(t);
}

char *reverse(char *s){
    int i, j;
    char c;
    
    for (i = 0, j = strlen(s) - 1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
    return s;
}