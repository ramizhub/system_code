/*
    This program imitates Linux BASH terminal (in particular,the ability to run commands). 
    Programm was written in Ubuntu 20.04.3 LTS.
  
  ┌─────────────────────┐
  │$: ./a.out           │
  │[ ]                  │          main()
  └─┬───────────────────┘          ┌──────────────────────────────────────────────────────┐
    │                              │                                                      │
    │                              │ RASH main function:                                  │
    │                              │ 1) Parse full line and split it ◄──────────────────┐ │
    │                              │ into commandname/path and keys                     │ │
    └─────────────────────────────►│                                                    │ │
                                   │ 2) Try to find binary of command                   │ │
    ┌──────────────────────────────┤                                                    │ │
    │                              │ 3) Split programm flow into child and              │ │
    │                              │ parent process. In first one we run                │ │
    │                              │ binary. In second one we wait for end of launch.   │ │
    │                              │ Wait for another input and prepare program for     │ │
    │                              │ another iteration.                                 │ │
    │                              │                │                                   │ │
    │                              │                └──────────────────────────────────►  │
    │                              │                                                      │
┌───┼──────────────────────────────┴──────────────────────────────────────────────────────┴┐
│   │                                                ▲                                   ▲ │
│   │                                                │ return this abosulte binary path  │ │
│   ▼                                                └─                                  │ │
│   findPath(environment PATH variable, commandname/path)                                │ │
│             │                                                                          │ │
│             │                                                                          │ │
│             │split this PATH into paths                                                │ │
│             │                                                                          │ │
│             │                                                                          │ │
│             └────►    We turn every commandname into path and check its existence      │ │
│                                                                                        │ │
│                    If it exists we return it to main() ────────────────────────────────┘ │
│                    Else we return NULL, main will deal with it itself                    │
│                                                                                          │
│                                                                                          │
└──────────────────────────────────────────────────────────────────────────────────────────┘
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <limits.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define SLASH "/"
#define charSLASH '/'
#define COLON ':'
#define BUFFMAX PATH_MAX

static char *findPath( char *envPATH, char *commname ) {
    /*
    *Calculation of the number of colons in environment variable i.e. loop iterations
    *Loop. In every iteration we have one of many possible paths, that we take from environment variable
    *Then we make a possible path where heap reqpath is directory name and commandname is commandname. Check this one for existence
    *Clean up and continue search
    *If we cannot find existing binary for command we return NULL to main() function
    */
    
    if(commname[0] == charSLASH)                            /* Path check. one(i.e first) character is enough */
        return commname;                                    /* Absolute path doen't need checking for existence */
    
    struct stat fstat;                                      /* Structure that contains some info about file, we just need return value from stat() */
    _Bool isBinary = false;                                 /* At the moment we make existing binary path we exit from loop with this bool variable */
    char *reqpath;                                          /* Required path. at first it is some directory name, then it is a path*/
    int ct = 0;                                             /* Const. relative const variable because it runs along enitre path*/
    int v = 0;                                              /* Volatile variable that resets to zero in every iteration, cause reqpath changes in every iteration */
    int coloncount = 1;                                     /* Colon count */
    int loopindex = 0;                                      /* For the desired control we need exactly as many iterations as there are words in the path */

    for(int i = 0; i < strlen(envPATH); i++)                
        if(envPATH[i] == COLON)                             /* Colon couning */ 
            coloncount++;                                  

    while(loopindex < coloncount) {
        reqpath = calloc(1000, sizeof(char));              
        while(envPATH[ct] != COLON) {                       /* Take bytes till colon meeting and work with resulting path */
            reqpath[v] = envPATH[ct];
            ct++;
            v++;
        }

        if(strlen(reqpath) + strlen(commname) + strlen(SLASH) + 1 <= BUFFMAX) {
            strcat(reqpath, SLASH);
            strncat(reqpath, commname, strlen(commname));
        } else {
            fprintf(stderr, "Not enough memory to make %s file in %s directory adress", commname, reqpath);
            free(reqpath);
            return NULL;
        }

        if(stat(reqpath, &fstat) == 0) {
            isBinary = true;                                /* Check self-made path for existence */
            return reqpath;
        }  
        
        v = 0, ct++, loopindex++;
        free(reqpath);
    }
    return NULL;                                            /* We didn't find binary earlier so return NULL to main() function */
}

int main(void) {
    pid_t pid;                                              /* Process id, we need it for waitpid() too */
    char *reqpath;                                          /* In main() function reqpath is findPath() return value i.e binary file */
    char *newargv[100];                                     /* Execve() second argument. [0] is commandname, rest are keys */
    char *newenvp[] = {NULL};                               /* Execve() third argument, usually is sent as NULL */
    char *envPATH = getenv("PATH");                         /* Get an environment variable */

    if(envPATH == NULL) {
        fprintf(stderr, "getenv() function couldn't find environment variable PATH");                   /* Can't get environment variable */
        exit(EXIT_FAILURE);
    }
                   
    char ckcarray[100][BUFFMAX];                            /* Due to some bugs, it is temporary container between heap and newargv[] */
    char full_line[BUFFMAX];                                /* Full command line that contains commandname/path and keys */
    char *tempp = calloc(BUFFMAX, sizeof(char));            /* Heap that takes one word from command line in every iteration of commandline treatment loop */
    char ch;                                                /* Input unit */
    
    int ckcount = 0;                                        /* Command and Keys count. Needed to process the exact number of keys */
    int rdcount = 0;                                        /* Readed bytes count. Needed for taking the necessary words with the sscanf() function */ 
    int i = 0;                                              /* Just a supporting variable */
    memset(full_line, '\0', BUFFMAX);                       /* Cleaning array to avoid */
    
    while((ch = getchar()) != '\n') {                       /* User input moment. */
        full_line[i++] = ch;
        if(i == sizeof(full_line)) {                        /* Edge case in which a huge number of characters are entered */
            fprintf(stderr, "Input limit exceeded.");       
            exit(EXIT_FAILURE);
        }
    }

    i = 1;                                                  /* First cell is for commandname, starting at index 1 for keys */
    while(sscanf(full_line + rdcount, "%s", tempp) != EOF && strcasecmp(full_line, "exit") != 0) {          /* "exit" and "EXIT" are exit words */                          
        if(strlen(ckcarray[0]) + strlen(tempp) + 1 <= BUFFMAX)
            strncpy(ckcarray[0], tempp, strlen(tempp) + 1);                                                 /* Commandname/path processing starts */
        else {
            fprintf(stderr, "Not enough memory to contain your %s commandname", tempp);
            exit(EXIT_FAILURE);
        }
        
        rdcount += strlen(tempp) + 1;                       
        ckcount++;
        while(sscanf(full_line + rdcount, "%s", tempp) != EOF) {                                            /* Key processing loop */
            if(strlen(ckcarray[i]) + strlen(tempp) + 1 <= BUFFMAX)
                strncpy(ckcarray[i], tempp, strlen(tempp) + 1); 
            else {
                fprintf(stderr, "Not enough memory to contain your %s commandname", tempp);
                exit(EXIT_FAILURE);
            }
                        
            i++, ckcount++;
            rdcount += strlen(tempp) + 1;
        }

        for(i = 0; i < ckcount; i++)                        /* Taking parsed command line by newargv */
            newargv[i] = ckcarray[i];

        reqpath = findPath(envPATH, newargv[0]);            /* newargv[0] first argument (asbolutepath/commandname) treatment */
        
        if(reqpath == NULL) {
            fprintf(stderr, "%s: command not found", newargv[0]);
            exit(EXIT_FAILURE);                             /* Dissability of findPath() to find your binary */
        }

        pid = fork();                                       /* Split remaining piece of code for two processes */
        if(pid == - 1) {                                    /* Error case */
            fprintf(stderr, "Internal fault");
            perror(NULL);
            exit(EXIT_FAILURE);
        } 
        else if(pid == 0) {
            execve(reqpath, newargv, newenvp);              /* Run binary */
        
            fprintf(stderr, "Can't execute your command."); 
            perror(NULL);                                   /* This piece of code is activated on an error with the running of the binary */
            exit(EXIT_FAILURE);
        }
        else {
            if(waitpid(0, NULL, 0) == -1) {                 /* We wan't to wait for running program by child process and then get ready for another iteration */
                fprintf(stderr, "Internal fault");
                exit(EXIT_FAILURE);
            }
            
            for(i = 0; i < ckcount; i++) 
                newargv[i] = NULL;                          /* Cleaning newargv[] for the next iteration to work with its own data. */
            i = 0;
            memset(full_line, '\0', BUFFMAX);               /* Cleaning commandline */
            
            while((ch = getchar()) != '\n') {
                full_line[i++] = ch;
                if(i == sizeof(full_line)) {
                    fprintf(stderr, "Input limit exceeded.");
                    exit(EXIT_FAILURE);
                }
            }
            i = 1, rdcount = 0, ckcount = 0;
        }
    }
    free(tempp);                                            /* When "exit" or "EXIT" printed we need to clean never cleaned up heap */
    return 0;
}