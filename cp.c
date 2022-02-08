#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h> 

#include <dirent.h>


struct stat src_stat;
struct stat trg_stat;
struct dirent * directory_stat;

_Bool last_object_file = false;
_Bool last_object_directory = false;

#define AVERAGE 500

int recfunc( char * current_object, char * last_object );


int main(int argc, char * argv[])
{
    if(argc < 3)
    {
        puts("cp: error in usage: compiled_program  source_objects  target_object");                                    // USAGE CHECKING
        exit(EXIT_FAILURE);
    }

    
    
    
    if( stat(argv[argc -1], &trg_stat) != 0)
    {
        perror("cp: error in stat function, working with last object: ");                                               // STAT LAST OBJECT CHECKING
        exit(EXIT_FAILURE);
    }

    
    
    
    if( (trg_stat.st_mode & S_IFMT) == S_IFREG )
    {
        last_object_file = true;
    } else if( (trg_stat.st_mode & S_IFMT) == S_IFDIR )                                                                 // FIGURE OUT WHAT IS OUR LAST OBJECT: FILE OR DIRECTORY
    {
        last_object_directory = true;
    }

    
    

    if( last_object_file == true )
        for(int reindex = 0; reindex < argc - 1; reindex++)
        {
            if( stat( argv[reindex], &src_stat) != 0 )
            {
                perror("cp: error in stat function, working with src object: ");
                exit(EXIT_FAILURE);                                                                                     // LOOKING FOR NONFILE OBJECTS, THAT I CAN'T COPY IN FILE
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
        {                                                                                                               // COPY FILES IN FILE 
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
        char current_object_name[AVERAGE];                                                          // NAMES FOR ARGV == RECFUNC ARGUMENTS                          
        char last_object_name[AVERAGE];                                                                                       
                                                                                                                             
        DIR * trgdp = opendir( argv[argc - 1] );                                                    
        if( trgdp == NULL )
        {
            perror("cp: error in opendir function, working with target directory");                 // CHECKING LAST DIRECTORY FOR WORKING                
            exit(EXIT_FAILURE);
        }                                                                                                               

        for( int index = 1; index < argc - 1; index++)
        {
            strcpy( current_object_name, argv[index] );                                             // REFRESHING RECFUNC ARGUMENTS
            strcpy( last_object_name, argv[argc - 1] );
            recfunc( current_object_name, last_object_name );
        }
    }
}




int recfunc( char * current_object, char * last_path )
{
    int size;   int slashcount = 0;
    lstat( current_object, &src_stat );
    if( (src_stat.st_mode & S_IFMT) == S_IFREG )                                                    // WORKING WITH FILE -> DIRECTORY
    {
        char currentobject_copy[AVERAGE];       strcpy( currentobject_copy, current_object );       // CREATING A COPY OF FILE'S NAME
        size = strlen( current_object );
        slashcount = 0;

        
        for( int reindex = 0; reindex < size; reindex++ )
            if( current_object[reindex] == '/' )                                                    // LOOKING FOR '/' IN FILE'S NAME ( in a path or in a word )
                slashcount++;

        if( slashcount == 0 )                                                                       // word case
        {
            strcat( last_path, "/" );
            strcat( last_path, current_object );
        } else if( slashcount != 0 )                                                                // path case
        {
            char * temp_pt;
            for( int reindex = 0; reindex < slashcount; reindex++ )
            {   
                temp_pt = strstr( currentobject_copy, "/" );
                if( reindex == slashcount-1 )
                    break;

                temp_pt[0] = '`';
            }

            strcpy( currentobject_copy, current_object );
            strcat( last_path, temp_pt );
        }
        

        FILE * srcfp = fopen( current_object, "r" );
        if( srcfp == NULL )
        {
            perror("cp: error in source file opening: ");
            exit(EXIT_FAILURE);
        }                                                                                           // 1) OPENING EXISTING ( source ) FILE 2) OPENING UNKNOWN FILE WITH THIS NAME
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

        
        char currentobject_copy[AVERAGE];   char lastpath_copy[AVERAGE];                            // COPIES FOR MEMORIZATION ( need after recfunc call )
        strcpy( currentobject_copy, current_object );   strcpy( lastpath_copy, last_path );

        size = strlen( current_object );    slashcount = 0;

        
        for( int reindex = 0; reindex < size; reindex++ )
            if( current_object[reindex] == '/' )                                                    // LOOKING FOR '/' IN DIR'S NAME ( in a path or in a word)               
                slashcount++;

        if( slashcount == 0)                                                                        // word case
        {
            strcat( last_path, "/" );
            strcat( last_path, current_object );
        } else if( slashcount != 0)                                                                 // path case
        {
            char * temp_pt;
            for( int reindex = 0; reindex < slashcount; reindex++ )
            {
                temp_pt = strstr( currentobject_copy, "/" );
                if( reindex == slashcount-1 )
                    break;
                
                temp_pt[0] = '`';
            }

            strcpy( currentobject_copy, current_object );
            strcat( last_path, temp_pt );
        }
        
        strcpy( lastpath_copy, last_path );                                                         // MEMORIZATION LAST PATH IN HIS COPY
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
            strcat( current_object, directory_stat->d_name );                                       // CRAFTING PATH WITH DIR'S NAME AND HIS FILE'S NAME 
            recfunc( current_object, last_path );
            strcpy( last_path, lastpath_copy );                                                     // PULL VALUES FROM COPIES
            strcpy( current_object, currentobject_copy );
        }
    }


    if( (src_stat.st_mode & S_IFMT) == S_IFLNK )
    {
        slashcount = 0;     size = strlen( current_object );
        char currentobject_copy[AVERAGE];   strcpy( currentobject_copy, current_object );
        char linkcontent[AVERAGE];
        
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
            char * temp_pt;
            for( int reindex = 0; reindex < slashcount; reindex++ )
            {   
                temp_pt = strstr( current_object, "/" );
                if( reindex == slashcount - 1)
                    break;
            
                temp_pt[0] = '`';
            }
            strcpy( current_object, currentobject_copy );
            strcat( last_path, temp_pt );
        }
        
        readlink( current_object, linkcontent, AVERAGE );
        symlink( linkcontent, last_path );
    }
}