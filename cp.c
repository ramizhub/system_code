#include <stdio.h>
#include <stdbool.h>

#include <time.h> 

#include <sys/types.h>
#include <sys/stat.h> 

#include <dirent.h>
#include <pwd.h>
#include <grp.h>

int main( int argc, char * argv[] )
{
    if( argc < 3)
    {
        puts("cp: usage: compiled_program source target");                                         // looking for arguments count 
        return 1;
    }

    struct stat standard_object_stat;
    struct stat last_object_stat;

    for(int index = 1; index < argc; index++)
    {
        if( stat(argv[index], &standard_object_stat) != 0)
        {
            perror("cp: error in stat function: ");                                                 // taking data from every argument and searching bugs
            return 1;
        }
    } 
 
    
    stat(argv[argc - 1], &last_object_stat);
    
    if( (last_object_stat.st_mode & S_IFMT) == S_IFREG )
    {
        for(int index = 1; index < (argc - 1) ; index++)
        {
            stat(argv[index], &standard_object_stat);
            if( (standard_object_stat.st_mode & S_IFMT) != S_IFREG)
            {
                puts("cp: invalid usage: only files can be copied in other files");                 // if last argument is file, rest need to be files, that is base 
                return 1;
            }    
        }
    }
       
    char read_buffer[40];

    if( (last_object_stat.st_mode & S_IFMT) == S_IFREG )                                            // if last argument is file) 
    {
        
        FILE * lastfp = fopen(argv[argc - 1], "a");                                                 // try to open last file
        if(lastfp == NULL)
        {
            perror("cp: error in file opening: ");                                                  // if i can't open last file
            return 1;
        }

        FILE * sdfp; 
        for(int index = 1; index < (argc - 1); index++)
        {
            sdfp = fopen(argv[index], "r");                                                         // try to open every file except last
            if(sdfp == NULL)
            {
                perror("cp: error in file opening: ");                                              // if i can't open one of them
                return 1;
            }
            
            while( fgets(read_buffer, 40, sdfp) != NULL )
            {                                                                                       // copy bytes to target
                fputs(read_buffer, lastfp);
            }            
        }
    } 
   
    







}