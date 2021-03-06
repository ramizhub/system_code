#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

#include <sys/types.h>
#include <sys/stat.h> 

#include <dirent.h>


struct stat src_stat;
struct stat trg_stat;
struct dirent * directory_stat;

_Bool last_object_file = false;
_Bool last_object_directory = false;


int recfunc( char * current_object, char * last_object );


int main(int argc, char * argv[])
{
    if(argc < 3)
    {
        puts("cp: error in usage: compiled_program  source_objects  target_object");                // USAGE CHECKING
        exit(EXIT_FAILURE);
    }

    
    
    
    if( stat(argv[argc -1], &trg_stat) != 0)
    {
        perror("cp: error in stat function, working with last object: ");                           // STAT LAST OBJECT CHECKING
        exit(EXIT_FAILURE);
    }

    
    
    
    if( (trg_stat.st_mode & S_IFMT) == S_IFREG )
    {
        last_object_file = true;
    } else if( (trg_stat.st_mode & S_IFMT) == S_IFDIR )                                             // FIGURE OUT WHAT IS OUR LAST OBJECT: FILE OR DIRECTORY
    {
        last_object_directory = true;
    }

    
    

    if( last_object_file == true )
        for(int reindex = 0; reindex < argc - 1; reindex++)
        {
            if( stat( argv[reindex], &src_stat) != 0 )
            {
                perror("cp: error in stat function, working with src object: ");
                exit(EXIT_FAILURE);                                                                 // LOOKING FOR NONFILE OBJECTS
            } if( (src_stat.st_mode & S_IFMT) != S_IFREG )   
            {
                puts("cp: error in usage: u can't copy nonfile objects in file");
                exit(EXIT_FAILURE);
            }
        }
    
    
    
    
    if( last_object_file == true )
    {
        FILE * trgfp = fopen( argv[argc-1], "a+" );    
        FILE * srcfp;
        char BUFFER[10];
        
        if( trgfp == NULL )
        {
            perror("cp: error in fopen function, working with target file");
            exit(EXIT_FAILURE);
        }

        for(int reindex = 1; reindex < argc - 1; reindex++)
        {                                                                                           // COPY FILES IN FILE 
            srcfp = fopen( argv[reindex], "r" );
            if(srcfp == NULL)
            {
                perror("cp: error in fopen function, working with src file");
                exit(EXIT_FAILURE);
            }

            while( fgets(BUFFER, 10, srcfp) != NULL )
                fputs(BUFFER, trgfp );
        }
    }




    if( last_object_directory == true )
    {
        char current_object_name[PATH_MAX];                                                         // NAMES RECFUNC ARGUMENTS                          
        char last_object_name[PATH_MAX];                                                                                       
                                                                                                                             
        DIR * trgdp = opendir( argv[argc - 1] );                                                    
        if( trgdp == NULL )
        {
            perror("cp: error in opendir function, working with target directory");                               
            exit(EXIT_FAILURE);
        }                                                                                                               

        for( int index = 1; index < argc - 1; index++)
        {
            strncpy( current_object_name, argv[index], strlen(argv[index]) + 1 );                   // REFRESHING RECFUNC ARGUMENTS
            strncpy( last_object_name, argv[argc - 1], strlen(argv[argc-1]) + 1 );
            recfunc( current_object_name, last_object_name );
        }
    }
}




int recfunc( char * current_object, char * last_path )
{
    int size;   
    int slashcount = 0;
    lstat( current_object, &src_stat );
    if( (src_stat.st_mode & S_IFMT) == S_IFREG )                                                    // WORKING WITH FILE -> DIRECTORY
    {
        char currentobject_copy[PATH_MAX];       
        strncpy( currentobject_copy, current_object, strlen(current_object) + 1 );                  // CREATING A COPY OF FILENAME
        size = strlen( current_object );
        slashcount = 0;

        
        for( int reindex = 0; reindex < size; reindex++ )
            if( current_object[reindex] == '/' )                                                    // LOOKING FOR '/' IN FILENAME ( in a path or in a word )
                slashcount++;

        if( slashcount == 0 )                                                                       // word case
        {
            strcat( last_path, "/" );
            strcat( last_path, current_object );
        } else if( slashcount != 0 )                                                                // path case
        {
            char * temp_pt = strstr( currentobject_copy, "/" );
            for( int reindex = 0; reindex < slashcount - 1; reindex++ )
                temp_pt = strstr( temp_pt + 1, "/" );
            

            strncpy( currentobject_copy, current_object, strlen(current_object) + 1 );
            strcat( last_path, temp_pt );
        }
        

        FILE * srcfp = fopen( current_object, "r" );
        if( srcfp == NULL )
        {
            perror("cp: error in source file opening: ");
            exit(EXIT_FAILURE);
        }                                                                                           // 1) OPENING EXISTING ( source ) FILE 2) OPENING UNKNOWN FILE WITH FILENAME
        FILE * trgfp = fopen( last_path, "a+" );
        if( trgfp == NULL )
        {
            perror("cp: error in target file opening: ");
            exit(EXIT_FAILURE);
        }

        char BUFFER[10];
        while( fgets(BUFFER, 10, srcfp ) != NULL )
            fputs( BUFFER, trgfp );

        
        if( fclose(srcfp) != 0 )
        {
            perror("cp: error in source file closing: ");
            exit(EXIT_FAILURE);
        } if( fclose(trgfp) != 0 )
        {
            perror("cp: error in target file closing: ");
            exit(EXIT_FAILURE);
        }
    }


    if( (src_stat.st_mode & S_IFMT) == S_IFDIR )                                                    // WORKING WITH DIRECTORY -> DIRECTORY
    {
        if( current_object[0] == '.' )                                                              // IGNORE '.' and '..' HIDDEN FOLDERS
            return 1;

        
        char currentobject_copy[PATH_MAX];   
        char lastpath_copy[PATH_MAX];                                                               // COPIES FOR MEMORIZATION ( need after recfunc call )
        strncpy( currentobject_copy, current_object, strlen(current_object) + 1 );   
        strncpy( lastpath_copy, last_path, strlen(last_path) + 1 );

        size = strlen( current_object );    slashcount = 0;

        
        for( int reindex = 0; reindex < size; reindex++ )
            if( current_object[reindex] == '/' )                                                    // LOOKING FOR '/' IN DIRNAME ( in a path or in a word )               
                slashcount++;

        if( slashcount == 0)                                                                        // word case
        {
            strcat( last_path, "/" );
            strcat( last_path, current_object );
        } else if( slashcount != 0)                                                                 // path case
        {
            char * temp_pt = strstr( currentobject_copy, "/" );
            for( int reindex = 0; reindex < slashcount - 1; reindex++ )
                temp_pt = strstr( temp_pt + 1, "/" );

            strncpy( currentobject_copy, current_object, strlen(current_object) + 1 );
            strcat( last_path, temp_pt );
        }
        
        strncpy( lastpath_copy, last_path, strlen(last_path) + 1 );                                 // MEMORIZATION LAST PATH IN HIS COPY
        mkdir( last_path, 4095 );

        
        
        DIR * srcdp = opendir( current_object );
        if( srcdp == NULL )
        {
            perror("cp: error in source directory opening: ");                                      // OPEN THIS DIRECTORY AND COPY FROM IT 
            exit(EXIT_FAILURE);
        }
    
        while( (directory_stat = readdir(srcdp) )  != NULL  )
        {
            if( directory_stat->d_name[0] == '.' )
                continue;

            strcat( current_object, "/" );
            strcat( current_object, directory_stat->d_name );                                       // CRAFTING PATH WITH DIRNAME AND FILENAME 
            recfunc( current_object, last_path );
            strncpy( last_path, lastpath_copy, strlen(lastpath_copy) + 1 );                         // PULL VALUES FROM COPIES
            strncpy( current_object, currentobject_copy, strlen(currentobject_copy) + 1 );
        }
    }


    if( (src_stat.st_mode & S_IFMT) == S_IFLNK )                                                    // WORKING WITH LINK -> DIRECTORY
    {
        slashcount = 0;     
        size = strlen( current_object );
        char currentobject_copy[PATH_MAX];   
        strncpy( currentobject_copy, current_object, strlen(current_object) + 1 );
        char linkcontent[PATH_MAX];
        
        for( int reindex = 0; reindex < size; reindex++ )
            if( current_object[reindex] == '/' )
                slashcount++;


        if( slashcount == 0 )
        {
            strcat( last_path, "/" );
            strcat( last_path, current_object );
        }

        if( slashcount != 0)
        {
            char * temp_pt = strstr( current_object, "/" );;
            for( int reindex = 0; reindex < slashcount - 1; reindex++ )
               temp_pt = strstr( temp_pt + 1, "/" );
            
            strncpy( current_object, currentobject_copy, strlen(currentobject_copy) + 1 );
            strcat( last_path, temp_pt );
        }
        
        if( readlink( current_object, linkcontent, PATH_MAX ) == -1 )
        {
            perror("cp: error in readlink function: ");
            exit(EXIT_FAILURE);                                                                     // CREATING A LINK
        } if( symlink( linkcontent, last_path ) != 0 )
        {
            perror("cp: error in symlink function: ");
            exit(EXIT_FAILURE);
        }
    
    }
}