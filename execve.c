#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <limits.h>

#include <sys/types.h>
#include <dirent.h>

#include <stdbool.h>

char * FindReqPath( char * envPATH, char * commname );

int 
main( int argc, char * argv[] )
{
    if( argc != 2 )
    {
        puts("usage: compiled_program command_name ");
        exit(EXIT_FAILURE);
    }

    pid_t fork_result;
    
    char * envPATH = getenv("PATH");    
    char * reqpath;
    char * newargv[] = { NULL };
    char * newenvp[] = { NULL };
    newargv[0] = argv[1];
    
    
    reqpath = FindReqPath( envPATH, argv[1] );

    if( reqpath == NULL )
    {
        puts("execve: error in searching command's binary ");
        exit(EXIT_FAILURE);
    }

    
    strcat( reqpath, "/" );
    strcat( reqpath, argv[1] );
    
    
    fork_result = fork();
    if( fork_result == - 1 )
    {
        perror("error in fork function: ");
        exit(EXIT_FAILURE);
    }

    if( fork_result == 0 )
        execve( reqpath, newargv, newenvp );
    else
        puts("Done!");
}


char * FindReqPath( char * envPATH, char * commname )
{
    
    
    struct dirent * dir_struct;
    DIR * dirp;
    
    char * reqpath;

    int ix1 = 0;
    int ix2 = 0;
    
    _Bool WEfoundpath = false;


    while( WEfoundpath != true )
    {
        reqpath = calloc( 1000, sizeof(char) );
        while( envPATH[ix1] != ':' )
        {
            reqpath[ix2] = envPATH[ix1];
            ix1++;
            ix2++;
        }

        dirp = opendir( reqpath );
        if( dirp == NULL )
        {
            perror("error in opening one of env adresses: ");
            exit(EXIT_FAILURE);
        }

        while( (dir_struct = readdir(dirp)) != NULL )
            if( strcmp( commname, dir_struct->d_name ) == 0 )
                WEfoundpath = true;       

        if( WEfoundpath == true )
            return reqpath;
    
        ix2 = 0;
        ix1++;

        if( ix1 > strlen( envPATH) && WEfoundpath != true )
            return NULL;


        free(reqpath);
    }

}
