#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


int main( int argc, char * argv[] )
{
    char BUFFER[10];    ssize_t readbytes;
    FILE * fp;    int fdescriptor;
    
    
    if(argc == 1)                                                               // one argument    ->   stdin - terminal, stdout - terminal
        while( fgets(BUFFER, 10, stdin) != NULL)                                
            fputs(BUFFER, stdout);

    else                                                                        // a few arguments ->   stdin - file, stdout - terminal
        for(int index = 1; index < argc; index++)
        {
            fp = fopen( argv[index], "r");
            
            if( fp == NULL )
            {
                perror("cat: error in file opening: ");                         
                exit(EXIT_FAILURE);
            }
            
            fdescriptor = fileno(fp);  
            
            while( (readbytes = read(fdescriptor, BUFFER, 9)) != 0 )            
            {
                BUFFER[readbytes] = '\0';
                fputs(BUFFER, stdout);
            }

            if( fclose(fp) != 0 )
            {
                perror("cat: error in file closing: ");
                exit(EXIT_FAILURE);
            }
        }
}