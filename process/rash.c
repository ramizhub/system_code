/*
    $: ./a.out (rash :D)
    [.] commandname
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/wait.h>

#define SLASH "/"
#define chSLASH '/'
#define COLON ':'
#define BUFFMAX 2048

char * findPath( char * envPATH, char * commname ) {

    struct stat fstat;
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
    char * reqpath;
    char * newargv[100];
    char * newenvp[] = {NULL};
    char * envPATH = getenv("PATH");
    
    if(envPATH == NULL) {
        puts("getenv() couldn't find environment paths");
        exit(EXIT_FAILURE);
    }

    char ch;                                                        // just a character
    char full_line[BUFFMAX];                                        // full comand line
    char ckcarray[100][BUFFMAX];                                    // due to problems with pointers to strings, i used this temporary container
    
    int rdcount = 0;                                                // readed bytes count
    int index = 0;                                                  // variable for small calculations
    int ckcount = 0;                                                // commands and keys count

    memset(full_line, '\0', BUFFMAX);
    while( (ch = getchar()) != '\n' ) 
        full_line[index++] = ch;
    
    index = 1;
    char * tempp = calloc(BUFFMAX, sizeof(char));                   // temporary container too
    
    while(sscanf(full_line + rdcount, "%s", tempp) != EOF 
    && strcmp(tempp, "exit") != 0 && strcmp(tempp, "EXIT") != 0) {
        
        // scan first element - absolute path or commandname
        rdcount += strlen(tempp) + 1;                               // indicate start for next scan
        strncpy(ckcarray[0], tempp, strlen(tempp) + 1);
        ckcount++;
        while(sscanf(full_line + rdcount, "%s", tempp) != EOF) {    // scan keys
            strncpy(ckcarray[index], tempp, strlen(tempp) + 1);
            index++;
            ckcount++;
            rdcount += strlen(tempp) + 1;
        }

        for(index = 0; index < ckcount; index++) 
            newargv[index] = ckcarray[index];

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
        if(pid == 0) {
            execve(reqpath, newargv, newenvp);
        
            perror("error: ");                      
            exit(EXIT_FAILURE);
        }
        else {
            waitpid(0, NULL, 0);
        
            for(index = 0; index < ckcount; index++ ) 
                newargv[index] = NULL;
            
            // preparing for next iteration
            rdcount = 0;
            index = 0;
            ckcount = 0;
            memset(full_line, '\0', BUFFMAX);
            while( (ch = getchar()) != '\n' ) 
                full_line[index++] = ch;
            index = 1;
        }

    }
}
