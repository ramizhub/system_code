#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <time.h>

void shredder( FILE * fp );


int main( int argc, char * argv[] )
{
    if( argc < 2 )
    {
        printf("shred: missing file operand");
        exit(EXIT_FAILURE);
    }
    

    int optional;   int hidecount = 3;
    _Bool verbose_option = false;
    _Bool remove_option = false;

    while( (optional = getopt(argc, argv, "huvn:")) != -1 )
        switch(optional) {
            case 'v':
                verbose_option = true;
                break;

            case 'u':
                remove_option = true;
                break;
            
            case 'n':
                if( optarg != NULL )
                { 
                    hidecount = atoi(optarg);
                    if( hidecount > 50 || hidecount < 1 )
                    {
                        printf("shred: invalid number of passse: '%s' ", optarg);
                        exit(EXIT_FAILURE);
                    }
                }
                break;

            case 'h':
                printf("Usage: shred [OPTION]... FILE...\n");
                printf("-h\tdisplay help and exit\n");
                printf("-n\toverwrite n-times\n");
                printf("-u\tremove vile after overwriting\n");
                printf("-v\tshow progress\n");
                exit(EXIT_FAILURE);
                break;
            default: 
                fprintf(stderr, "Try '-h' for more information");
                break;
        }

    struct stat object_struct;

    for( int reindex = optind; reindex < argc; reindex++ )
    {
        stat( argv[reindex] , &object_struct );
        if( (object_struct.st_mode & S_IFMT) != S_IFREG )
        {
            printf("shred %s: failed to open for writing: ", argv[reindex]);
            exit(EXIT_FAILURE);
        }
    }

    FILE * fp;   int character_count;
    
    for( int reindex = optind; reindex < argc; reindex++ )
    {
        character_count = 0;
        fp = fopen( argv[reindex] , "r" );
        if( fp == NULL )
        {
            perror("shred: error in file opening: ");
            exit(EXIT_FAILURE);
        }

        while( getc(fp) != EOF )
            character_count++;


        if( fclose(fp) != 0 )
        {
            perror("shred: error in file closing: ");
            exit(EXIT_FAILURE);
        }

        if( character_count == 0 )
            continue;


        fp = fopen( argv[reindex], "w" );

        for( int hide_index = 0; hide_index < hidecount; hide_index++ )
        {
            if( verbose_option == true )
                printf("shred %s: pass %d/%d ... \n", argv[reindex], hide_index + 1, hidecount);

            shredder( fp );
        }

        if( fclose(fp) != 0 )
        {
            perror("shred: error in file closing: ");
            exit(EXIT_FAILURE);
        }
    }

    if( remove_option == true )
        for( int reindex = optind; reindex < argc; reindex++ )
            if( unlink(argv[reindex]) != 0)
            {
                perror("shred: error in file removing: ");
                exit(EXIT_FAILURE);
            }

}






void shredder( FILE * fp )
{
    int limit, character, randomstart;
    long taketime;

    limit = rand()%1985 + 64;
    taketime = time(NULL);
    randomstart = (unsigned)taketime / 4;
    srand( randomstart );
    for( int reindex = 0; reindex < limit; reindex++ )
    {
        character = rand() % 94 + 33;
        if( character%2 == 0 && character%17 == 0)
            putc('\n', fp);

        fprintf(fp, "%c", character);
    }
}