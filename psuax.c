/*
    It is a program inspired by the ps command that works with a, u, x keys.
    All information is taken from the /proc folder. In particular, i parsed /proc/[PID]/stat , /proc/[PID]/statm files  and /proc/meminfo files.
    Some of original ps aux components presented in a different form.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include <limits.h>
#include <errno.h>

#include <pwd.h>                                                            
#include <grp.h>
#include <dirent.h>

#include <sys/types.h>                                                      
#include <sys/stat.h>

#include <sys/time.h>
#include <sys/resource.h>

#define SPACE ' '
#define DOT '.'

void find_ttyname( FILE * statfp );

int
main(void)
{
    DIR * dp_proc;                                  // "/proc" directory pointer
    DIR * dp_inner;                                 // "/proc/[pid]" directory pointer
    FILE * fp;                                      // just an external pointer to files
    FILE * fp_inner;                                // "/proc/[pid]/statm" file pointer

    if( (dp_proc = opendir("/proc")) == NULL )
    {
        perror("ps: error in /proc directory opening");
        exit(EXIT_FAILURE);
    }

    
    struct passwd * pass_struct;                    // password structure -> "USER"
    struct dirent * dir_struct;                     // readdir return value, use dir_struct.d_name for every object name in this directory
    struct stat lstat_struct;                       // checks every directory object for file type
    
    char path[PATH_MAX];                            // final result -> "/proc/[pid]/stat"
    char statmpath[PATH_MAX];                       // final result -> "/proc/[pid]/statm"
    char pstatechar, char2;                         // pstate ( I, R, S etc) and supporting character

    unsigned long pwtime_sec;                       // time, that process has been scheduled in user mode (clock ticks, so i need to divide it by sysconf(_SC_CLK_TCK))
    unsigned long vsz_kb;                           // virtual memory size in B ( so i need to divide it by 1024 to get kB )
    long int rss_kb;                                // resident set size ( page count, so i need to sysconf(_SC_PAGESIZE) and 1024 to get kB )
    unsigned long long starttime;                   // time the process started after system boot ( in my linux version it is clock ticks, so divide it by sysconf(_SC_CLK_TCK) )
    unsigned int hours, minutes, seconds;           // translate starttime to hh:mi:ss -> "START" is time the duration of the process in the format hh:mi:ss
    unsigned long phymemory_kb;                     // take this value from "/proc/meminfo" (first on the list) and divide rss_kb by this value to get %MEM
    unsigned long avialable;                        // variable, that keeps track of free space in path and statmpath array


    

//                                                              // LOOK FOR "MemTotal: X kB"  
    fp = fopen( "/proc/meminfo", "r" );
    if( fp == NULL )
    {
        perror("ps: error in /proc/meminfo file opening: ");
        exit(EXIT_FAILURE);
    }
    
    while( getc(fp) != SPACE )
        continue;
    
    if( fscanf(fp, "%lu", &phymemory_kb) != 1 )
    {
        perror("ps: can't read value of MemTotal in /proc/meminfo file: ");
        exit(EXIT_FAILURE);
    }
    
    if( fclose(fp) != 0 )
    {
        perror("ps: error in /proc/meminfo file closing: ");
        exit(EXIT_FAILURE);
    }

    
    
    
    printf("%s  %s  %s  %%%s\t%s\t%s\t%s\t%s\t\t%s\t%s\t%s\n", "USER", "PID", "CPU", "MEM", "VSZ", "RSS", "STAT", "START", "UTIME", "TTY", "COMMAND");



    
//                                                              GO IN "/proc" DIRECTORY AND PARSE EVERY PART OF IT
    while( (dir_struct = readdir(dp_proc)) != NULL )
    {
        if( dir_struct->d_name[0] == DOT )
            continue;
//                                                              CREATE 2 "/proc/[pid]" PATHS
        
        
        strncpy( path, "/proc/", strlen("/proc/") + 1 );
        if( PATH_MAX - strlen(path) >= 1 )
        {
            avialable = PATH_MAX - strlen(path) - 1;
            strncat( path, dir_struct->d_name, avialable );
        } else
        {
            printf("%s's array maxsize is not enough to take %s", path, dir_struct->d_name );
            exit(EXIT_FAILURE);
        }
        
        
        strncpy( statmpath, "/proc/", strlen("/proc/") + 1 );
        if( PATH_MAX - strlen(statmpath) >= 1 )
        {    
            avialable = PATH_MAX - strlen(statmpath) - 1;
            strncat( statmpath, dir_struct->d_name, avialable );
        } else
        {
            printf("%s's array maxsize is not enough to take %s", statmpath, dir_struct->d_name );
            exit(EXIT_FAILURE);
        }
        
        
        if( lstat(path, &lstat_struct) != 0 )
        {
            printf("\nps: error in lstat function with %s in first argument: ", path );
            perror(NULL);
            exit(EXIT_FAILURE);
        }

        
        if( S_ISDIR(lstat_struct.st_mode) )
        {
            dp_inner = opendir(path);
            if(dp_inner == NULL)
            {
                printf("\nps: error in %s directory opening: ", path );
                perror(NULL);
                exit(EXIT_FAILURE);
            }




//                                                              CREATE "/proc/[pid]/stat" AND "/proc/statm"
            if( PATH_MAX - strlen(path) >= 1 )
            {    
                avialable = PATH_MAX - strlen(path) - 1;
                strncat( path, "/stat", avialable );
            } else
            {
                printf("%s's array maxsize is not enough to take /stat", path );
                exit(EXIT_FAILURE);
            }
            
            
            if( PATH_MAX - strlen(statmpath) >= 1 )
            {
                avialable = PATH_MAX - strlen(statmpath) - 1;
                strncat( statmpath, "/statm", avialable );
            } else
            {
                printf("%s's array maxsize is not enough to take /statm", statmpath );
                exit(EXIT_FAILURE);
            }    
            

/*                                                              2 error cases in "/proc/[pid]/stat" file opening: 
                            
                            1) file exists and i can't open it  2) file does not exist and that is why i can't open it                              
                            
                            1) the first case is a catastrophic failure and we have to show an error message to the user and exit 
                                                                2) that is normal situation because not all directories are process directories  
*/


            fp = fopen( path, "r" );
            if( fp == NULL )
            {
                if( errno == ENOENT )
                    continue;
                else
                {
                    printf("ps: error in %s file opening: ", path);
                    perror(NULL);
                    exit(EXIT_FAILURE);
                }
            }

    
            pass_struct = getpwuid( lstat_struct.st_uid );
            if( pass_struct == NULL )
            {
                printf("\nps: error in getwpuid function with %d in argument: ", lstat_struct.st_uid );
                perror(NULL);
                exit(EXIT_FAILURE);
            } 
            
            

//                                                              LOOK FOR 14th ELEMENT OF "/proc/[pid]/stat" - utime                      
            for( int index = 0; index < 13; index++ )
                while( getc(fp) != SPACE )   
                    continue;

            
            if( fscanf(fp, "%lu", &pwtime_sec) != 1 ) 
            {
                printf("\nps: can't read utime in %s: ", path );
                perror(NULL);
                exit(EXIT_FAILURE);
            }
            
            pwtime_sec = pwtime_sec / sysconf(_SC_CLK_TCK);
            rewind(fp);




//                                                              LOOK FOR 3th ELEMENT OF "/proc/[pid]/stat" - pstate
            for( int index = 0; index < 2; index++ )
                while( getc(fp) != SPACE )
                    continue;
            
            if( fscanf(fp, "%c", &pstatechar) != 1 )
            {
                printf("\nps: can't read pstate in %s: ", path);
                perror(NULL);
                exit(EXIT_FAILURE);
            }
            
            rewind(fp);
            
            

            
//                                                              LOOK FOR 23th ELEMENT OF "/proc/[pid]/stat" - vsz(B)
            for( int index = 0; index < 22; index++ )
                while( getc(fp) != SPACE )
                    continue;
            
            if( fscanf(fp, "%lu", &vsz_kb) != 1 )
            {
                printf("\nps: can't read vsz in %s: ", path);
                perror(NULL);
                exit(EXIT_FAILURE);
            }
            vsz_kb = vsz_kb / 1024;
            rewind(fp);




//                                                              LOOK FOR SECOND ELEMENT IN "/proc/pid/statm" - rss 
            fp_inner = fopen( statmpath, "r" );
            if( fp_inner == NULL )
            {
                perror("ps: error in statm file opening: ");
                exit(EXIT_FAILURE);
            }
            
            while( getc(fp_inner) != SPACE )
                continue;
            if( fscanf(fp_inner, "%ld", &rss_kb) != 1 )
            {
                printf("can't read rss in %s: ", statmpath );
                perror(NULL);
                exit(EXIT_FAILURE);
            }

            rss_kb = rss_kb * sysconf(_SC_PAGESIZE);
            rss_kb = rss_kb / 1024;
            
            if( fclose(fp_inner) != 0 )
            {
                printf("error in %s file closing: ", statmpath );
                perror(NULL);
                exit(EXIT_FAILURE);
            }




//                                                              LOOK FOR 22th ELEMENT IN "/proc/pid/stat" - starttime (clock ticks)
            for ( int index = 0; index < 21; index ++ )
                while( getc(fp) != SPACE )
                    continue;

            if( fscanf(fp, "%llu", &starttime) != 1 )
            {
                printf("can't read starttime in %s: ", path);
                perror(NULL);
                exit(EXIT_FAILURE);
            }
            starttime = starttime / sysconf(_SC_CLK_TCK);
            seconds = starttime;
            
            hours = seconds / 3600;
            seconds = seconds % 3600;
            minutes = seconds / 60;
            seconds = seconds % 60;

            rewind(fp);




//                                                              PRINT INFO  
            
            printf("%s  %s    %lld    %.2f  ", pass_struct->pw_name, dir_struct->d_name, starttime, (float)(rss_kb * 4) / phymemory_kb);
            printf(" %lu\t%lu\t %c\t", vsz_kb, rss_kb, pstatechar);
            printf("%u:%u:%u\t\t%lu\t", hours, minutes, seconds, pwtime_sec / 60);




//                                                              LOOK FOR DEVICE NAME (description below)
            find_ttyname(fp);
            rewind(fp);




//                                                              LOOK FOR 2th ELEMENT IN "/proc/pid/stat" - comm
            while( (char2 = getc(fp)) != '(' )
                continue;
            while( (char2 = getc(fp)) != ')')
                putchar(char2);
            putchar('\n');


//                                                              CLOSE "/proc/[pid]/stat" FILE AND "/[pid]" DIRECTORY 
            if( fclose(fp) != 0 )
            {
                printf("ps: error in %s file closing: ", path);
                perror(NULL);
                exit(EXIT_FAILURE);
            } if( closedir(dp_inner) != 0 )
            {
                printf("ps: error in /proc/%s directory closing: ", dir_struct->d_name );
                perror(NULL);
                exit(EXIT_FAILURE);
            }
        }
    }

//                                                              CLOSE "/proc" DIRECTORY 
    if( closedir(dp_proc) != 0 )
    {
        perror("ps: error in /proc directory closing: ");
        exit(EXIT_FAILURE);
    }
}



// THIS FUNCTION COMPARES TTY_NR FROM "/proc/pid/stat" WITH RETURNED VALUE FROM STAT FUNCTION - st_rdev 
// IF THESE NUMBERS EQUAL TO EACH OTHER THEN PRINT THIS DEVICE NAME (in fact it will be ttyname)

void find_ttyname ( FILE * statfp )
{
    
    _Bool theretty = false;                                     // a certain terminal is responsible for this process
    int tty_nr;                                                 // controlling terminal of the process ( number )
    char devpath[PATH_MAX];                                     // "/dev/" -> "/dev/device_name" -> "/dev/"
    
    static DIR * ddp;                                           // "/dev" directory pointer                                   
    static struct dirent * dev_dirstruct;                       // readdir return value, use dev_dirstruct.d_name for every object name in this directory
    static struct stat devstat_struct;                          // returns representing device ID 
    
    
    
    
//                                                              LOOK FOR 7th OBJECT IN "/proc/pid/stat" - tty_nr
    for( int index = 0; index < 6; index++ )
        while( getc(statfp) != SPACE )
            continue;
    fscanf( statfp, "%d", &tty_nr );




//                                                              OPEN "/dev" DIRECTORY FOR WORKING WITH DEVICE FILES
    ddp = opendir( "/dev" );
    if( ddp == NULL )
    {
        perror("ps: error in /dev directory opening: ");
        exit(EXIT_FAILURE);
    }

    strncpy( devpath, "/dev/", strlen("/dev/") + 1 );




//                                                              PARSE EVERY PART OF "/dev" DIRECTORY
    while( (dev_dirstruct = readdir(ddp)) != NULL )
    {
//                                                              EXCEPT HIDDEN FOLDERS
        if( dev_dirstruct->d_name[0] == DOT )
            continue;
        
        
        
        if( strlen(devpath) + strlen(dev_dirstruct->d_name) + 1 <= PATH_MAX )
            strncat( devpath, dev_dirstruct->d_name, strlen(dev_dirstruct->d_name) );
        else
        { 
            printf("%s's array maxsize is not enough to take %s", devpath, dev_dirstruct->d_name );
            exit(EXIT_FAILURE);
        }
        
        
        
        if( stat( devpath, &devstat_struct ) != 0 )
            continue;

        
        if( devstat_struct.st_rdev == tty_nr && tty_nr != 0 )
        {
          printf("%s\t", dev_dirstruct->d_name );
          theretty = true;
          continue;
        } 
        
        strncpy( devpath, "/dev/", strlen("/dev/") + 1 );
    }

//                                                              DETERMINES WHETHER A DEVICE IS RESPONSIBLE FOR THIS PROCESS OR NOT
    if( theretty == false )
    {
        printf("?\t");
    } if( closedir(ddp) != 0 )
    {
        perror("error in /dev diretory closing: ");
        exit(EXIT_FAILURE);
    }   
}