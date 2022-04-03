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

char *findPath( char *envPATH, char *commname ) {
    struct stat fstat;
    _Bool isBinary = false;
    char *reqpath;
    int ct = 0;
    int v = 0;
    int slashcount = 0;

    for(int i = 0; i < strlen(commname); i++)
        if(commname[i] == charSLASH)
            return commname;

    while(isBinary != true) {
        reqpath = calloc(1000, sizeof(char));
        while(envPATH[ct] != COLON) {
            reqpath[v] = envPATH[ct];
            ct++;
            v++;
        }

        if(strlen(reqpath) + strlen(commname) + strlen(SLASH) + 1 <= BUFFMAX) {
            strcat(reqpath, SLASH);
            strncat(reqpath, commname, strlen(commname));
        } else {
            printf("%s has not enough size to concatenate %s\n", reqpath, commname);
            return NULL;
        }

        if(stat(reqpath, &fstat) == 0) {
            isBinary = true;
            return reqpath;
        }  
        v = 0, ct++;
        free(reqpath);
    }
    if(isBinary == false) 
        return NULL;
}

int main(void) {
    pid_t pid;    
    char *reqpath;
    char *newargv[100];
    char *newenvp[] = {NULL};
    char *envPATH = getenv("PATH");

    if(envPATH == NULL) {
        puts("getenv() couldn't find environment paths");
        exit(EXIT_FAILURE);
    }
                   
    char ckcarray[100][BUFFMAX];     
    char full_line[BUFFMAX];
    char *tempp = calloc(BUFFMAX, sizeof(char)); 
    char ch; 
    
    int ckcount = 0;  
    int rdcount = 0;  
    int i = 0;
    memset(full_line, '\0', BUFFMAX);
    
    while( (ch = getchar()) != '\n' ) 
        full_line[i++] = ch;
    i = 1;

    while(sscanf(full_line + rdcount, "%s", tempp) != EOF 
    && strcmp(tempp, "exit") != 0 && strcmp(tempp, "EXIT") != 0) {                               
        strncpy(ckcarray[0], tempp, strlen(tempp) + 1);
        
        rdcount += strlen(tempp) + 1;
        ckcount++;
        while(sscanf(full_line + rdcount, "%s", tempp) != EOF) {
            strncpy(ckcarray[i], tempp, strlen(tempp) + 1);
            
            i++, ckcount++;
            rdcount += strlen(tempp) + 1;
        }

        for(i = 0; i < ckcount; i++) 
            newargv[i] = ckcarray[i];

        reqpath = findPath(envPATH, newargv[0]);
        
        if(reqpath == NULL) {
            puts("could not find requested command in the PATH. try again:\n");
            exit(EXIT_FAILURE);
        }

        pid = fork();
        if(pid == - 1) {
            perror("error in fork function: ");
            exit(EXIT_FAILURE);
        } 
        else if(pid == 0) {
            execve(reqpath, newargv, newenvp);
        
            perror("error: ");                      
            exit(EXIT_FAILURE);
        }
        else {
            waitpid(0, NULL, 0);
            
            for(i = 0; i < ckcount; i++) 
                newargv[i] = NULL;
            i = 0;
            memset(full_line, '\0', BUFFMAX);
            while((ch = getchar()) != '\n') 
                full_line[i++] = ch;
            
            rdcount = 0, i = 1, ckcount = 0;
        }
    }
}
