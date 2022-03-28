/*
 ┌───────────────────────────────────────┐
 │$: ./a.out commandname                 │
 │$: ./a.out commandpath                 │
 └──────────────────────┬────────────────┘
                        │                          ┌────────────────────────────────────────────────────────────┐
                        │env PATH, command         │ Break env PATH into the parts                              │
                        └────────────────────────► │    │                                                       │
                                 findPath()        │    │ if commandpath is given                               │
                                                   │    ├──────────────────────────────►  return commandpath    │
                                                   │    │                                                       │
                                                   │    │                                                       │
                                                   │    │                                                       │
                                                   │    │ if commandname is given         create a binary path  │
┌────────────────────────────────┐                 │    └──────────────────────────────►  check it for existence│
│ findPath()                     │                 │                                       │     │              │
│                                │                 │                   couldn't find       │     │              │
│ LOOP                           │                 │return NULL      ◄─────────────────────┘     │              │
│ exit when user◄─────────────┐  │                 │                                             │              │
│ types "exit"                │  │                 │                                             │              │
│                             │  │                 │                   found binary              │              │
│                             │  │                 │return binary    ◄───────────────────────────┘              │
│ fork()                      │  │◄────────────────┤                                                            │
│ │                           │  │                 └────────────────────────────────────────────────────────────┘
│ │                           │  │
│ │                           │  │
│ │pid = 0                    │  │
│ ├─────────────►  run binary │  │
│ │                           │  │
│ │pid = -1                   │  │
│ ├─────────────►  exit       │  │
│ │                           │  │
│ │else                       │  │
│ └─────────────►  wait for child│
│                                │
└────────────────────────────────┘
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/wait.h>

#define SLASH "/"
#define chSLASH '/'
#define COLON ':'


char * findPath(char * envPATH, char * commname);

int 
main(int argc, char * argv[]) {
    
    if(argc < 2) {
        printf("usages:\n%s command_name", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    pid_t pid;    
    
    char * reqpath;
    char * newargv[] = { NULL };
    char * newenvp[] = { NULL };
    char * envPATH = getenv("PATH");  

    newargv[0] = argv[1];


    while(strcmp( argv[1], "exit") != 0 && strcmp( argv[0], "exit") != 0) {
        
        reqpath = findPath(envPATH, argv[1]);
        
        if(reqpath == NULL) {
            puts("could not find requested command in the PATH. ");
            exit(EXIT_FAILURE);
        }

        pid = fork();
        
        if(pid == - 1) {
            perror("error fork function: ");
            exit(EXIT_FAILURE);
        }

        if(pid == 0) {
            execve(reqpath, newargv, newenvp);
        
            perror("error: ");                      
            exit(EXIT_FAILURE);
        }
        else {
            waitpid(0, NULL, 0);
            scanf("%s", newargv[0]);
        }
    }

}


char * findPath(char * envPATH, char * commname) {
    
    FILE * fp;
    char * reqpath;
    int ct = 0;
    int v = 0;
    int slashcount = 0;
    _Bool isBinary = false;

    for(int index = 0; index < strlen(commname); index++)
        if(commname[index] == chSLASH)
            return commname;
    
    while(isBinary != true) {
        
        reqpath = calloc(1000, sizeof(char));
        while(envPATH[ct] != COLON) {
            reqpath[v] = envPATH[ct];
            ct++;
            v++;
        }

        if(strlen(reqpath) + strlen(commname) + strlen(SLASH) + 1 <= PATH_MAX) {
            strcat(reqpath, SLASH);
            strncat(reqpath, commname, strlen(commname));
        } else {
            printf("%s has not enough size to concatenate %s\n", reqpath, commname);
            return NULL;
        }

        fp = fopen(reqpath, "r");
        if(fp != NULL) {
            isBinary = true;
            return reqpath;
        }
        
        v = 0, ct++;
        free(reqpath);
    
    }
    if(isBinary == false) 
        return NULL;

}