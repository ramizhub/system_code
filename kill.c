/*
This program imitates Linux kill command. Programm was written in Ubuntu 20.04.3 LTS.

┌───────────────────────────────────────────────────────────────────────────────────────────┐
│ I have three possible cases:                                                              │
│                                                                                           │
│  1) $: ./a.out -l                -          print list of signal names                    │
│  2) $: ./a.out pid               -          kill pid process                              │
│  3) $: ./a.out sig pid           -          send signal (sig) to process (pid)            │
└───────────────────────────────────────────────────────────────────────────────────────────┘
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <signal.h>

void upall(char * str) {                                                        // up all characters in string( output of original kill )
    while(*str) {
        *str = toupper(*str);                                                   
        str++;
    }
}

void listsigs(void) {                                                           // printf list of signals and names
    int sig;
    for(sig = 1; sig < 32; sig++) {
        char *str = strdup(strsignal(sig));                                     // for proper memory allocation used strdup(), that is why i can use free() for this string
        if(!str)
            exit(EXIT_FAILURE);

        upall(str);
        if(sig % 5 == 0)                                                        
            printf("\n\n");
        
        printf("%d) %s\t", sig, str);
        
        free(str);
    } 
}

int main(int argc, char * argv[]) {
    
    if(argc < 2) {
        puts("usage: cprogram pid");
        exit(EXIT_FAILURE);
    }
    
    pid_t pid;
    int sig;

    if(argc == 2) {                                                             // compiled_program -l
        if(strcmp(argv[1], "-l") == 0 )
            listsigs();
        else {                                                                  // compiled_program pid (kill process)
            pid = atoi(argv[1]);
            if(kill(pid, SIGKILL) != 0) {
                perror("error kill: ");
                exit(EXIT_FAILURE);
            }
        }
    } else {                                                                    // compiled_program sig pid
        pid = atoi(argv[2]);
        sig = atoi(argv[1]);
        if(kill(pid, sig) != 0) {
            perror("error kill: ");
            exit(EXIT_FAILURE);
        }
    }
}