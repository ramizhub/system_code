/*
┌────────────────────────────────┐
│ bash: ./a.out commandname      │
│                                │
│                                │
└──────────────────┬─────────────┘                       ┌──────────────────────────────────┐
                   │                                     │ break PATH into the parts        │
                   │                                     │  │                               │
                   │   env PATH, commandname             │  │                               │
                   └──────────────────────────────────►  │  └───► search binary by adresses │
                                                         │              │                   │
                                                         │              │                   │
                                                         │              │                   │
                                                         │return NULL ◄─┴─► return required │
                                                         │                         binary   │
                                                         │                                  │
┌─────────────────────────────────────┐                  │                                  │
│ LOOP ( exit with "exit" )           │                  └────────────┬─────────────────────┘
│    fork()                           │                               │
│     │    │      child process       │                               │
│     │    └────►   exec binary       │                               │
│     │                               │    NULL or required  binary   │
│     │                               │◄──────────────────────────────┘
│     │      parent process           │
│     └───►    wait for child         │
│                                     │
│                                     │
│                                     │
└─────────────────────────────────────┘
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
#define COLON ':'


char * findPath( char * envPATH, char * commname );

int 
main( int argc, char * argv[] ) {
    
    if( argc < 2 ) {
        puts("usage: compiled_program command_name");
        exit(EXIT_FAILURE);
    }
    
    pid_t pid;    
    
    char * reqpath;
    char * newargv[] = { NULL };
    char * newenvp[] = { NULL };
    char * envPATH = getenv("PATH");  

    newargv[0] = argv[1];


    while( strcmp( argv[1], "exit") != 0 && strcmp( argv[0], "exit") != 0 ) {
        
        reqpath = findPath( envPATH, argv[1] );
        
        if( reqpath == NULL ) {
            puts("execve: error in searching command's binary ");
            exit(EXIT_FAILURE);
        }

    
        if( strlen(reqpath) + strlen(argv[1]) + strlen(SLASH) + 1 <= PATH_MAX ) {
            strcat( reqpath, SLASH );
            strncat( reqpath, argv[1], strlen(argv[1]) );
        } else {
            printf("%s has not enough size to concatenate %s\n", reqpath, argv[1] );
            exit(EXIT_FAILURE);
        }


        pid = fork();
        if( pid == - 1 ) {
            perror("error fork function: ");
            exit(EXIT_FAILURE);
        }

        if( pid == 0 ) {
            execve( reqpath, newargv, newenvp );
        
            perror("error: ");                      
            exit(EXIT_FAILURE);
        }
        else {
            waitpid( 0, NULL, 0 );
            scanf("%s", newargv[0] );
        }
    }
}


char * findPath( char * envPATH, char * commname ) {
    
    DIR * dirp;
    struct dirent * dirstruct;
    
    char * reqpath;
    
    int ct = 0;
    int v = 0;
    
    _Bool isBinary = false;

    while( isBinary != true ) {
        
        reqpath = calloc( 1000, sizeof(char) );
        while( envPATH[ct] != COLON ) {
            reqpath[v] = envPATH[ct];
            ct++;
            v++;
        }

        dirp = opendir( reqpath );
        
        if( dirp == NULL ) {
            perror("error in opening one of env adresses: ");
            exit(EXIT_FAILURE);
        }

        while( (dirstruct = readdir(dirp)) != NULL )
            if( strcmp( commname, dirstruct->d_name ) == 0 )
                isBinary = true;       

        if( isBinary == true ) 
            return reqpath;
    
        v = 0, ct++;

        if( ct > strlen( envPATH) && isBinary != true )
            return NULL;
        
        free(reqpath);
    }
}