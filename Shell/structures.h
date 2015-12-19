#ifndef structures_h
#define structures_h

struct programm{
    char *name;
    int numberOfArguments;
    char **arguments;
    char *inputFile, *outputFile;
    int outputType; // 1 - rewrite; 2  - append
};

struct job{
    int background;
    struct programm *programs;
    int numberOfPrograms;
};

struct environmentVariables{
    char **argv;
    int argc;
    int statusOfLastProcess;
    char *username;
    char *homeDirectory;
    char *shellDirectory;
    int UID;
    char *currentDirectory;
    int PID;
};

#endif
