/*
    It is a program inspired by the ps command that works with a, u, x keys.
    All information is taken from the /proc folder. In particular, i parsed /proc/[PID]/stat , /proc/[PID]/statm files  and /proc/meminfo files.
    Some of original ps aux components presented in a different form.
*/

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include <pwd.h>                                                            
#include <grp.h>

#include <sys/types.h>                                                      
#include <sys/stat.h>

#include <sys/time.h>
#include <sys/resource.h>

#define SPACE ' '

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
    struct stat lstat_struct;                       // check every directory object for file type
    
    char path[PATH_MAX];                            // final result -> "/proc/[pid]/stat"
    char statmpath[PATH_MAX];                       // final result -> "/proc/[pid]/statm"
    char pstatechar, char2;                         // pstate ( I, R, S etc) and supporting character

    unsigned long pwtime_sec;                       // time, that process has been scheduled in user mode (clock ticks, so i need to divide it by sysconf(_SC_CLK_TCK))
    unsigned long vsz_kb;                           // virtual memory size in B ( so i need to divide it by 1024 to get kB )
    long int rss_kb;                                // resident set size ( page count, so i need to sysconf(_SC_PAGESIZE) and 1024 to get kB )
    unsigned long long starttime;                   // time the process started after system boot ( in my linux version it is clock ticks, so divide it by sysconf(_SC_CLK_TCK) )
    unsigned int hours, minutes, seconds;           // translate starttime to hh:mi:ss -> "START" is time the duration of the process in the format hh:mi:ss
    unsigned long phymemory_kb;                     // take this value from "/proc/meminfo" (first on the list) and divide rss_kb by this value to get %MEM



    

//                                                              // LOOK FOR "MemTotal: X kB"  
    fp = fopen("/proc/meminfo", "r");
    if( fp == NULL )
    {
        perror("ps: error in /proc/meminfo file opening: ");
        exit(EXIT_FAILURE);
    }
    
    while( getc(fp) != SPACE)
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

    
    
    
    printf("%s  %s  %s  %%%s\t%s\t%s\t%s\t%s\t\t%s\t%s\n", "USER", "PID", "CPU", "MEM", "VSZ", "RSS", "STAT", "START", "UTIME", "COMMAND");



    
//                                                              GO IN "/proc" DIRECTORY AND PARSE EVERY PART OF IT
    while( (dir_struct = readdir(dp_proc)) != NULL )
    {
        if( dir_struct->d_name[0] == '.' )
            continue;
//                                                              CREATE 2 "/proc/[pid]" PATHS
        strncpy( path, "/proc/", strlen("/proc/") + 1 );
        strncat( path, dir_struct->d_name, strlen(dir_struct->d_name) + 1 );
        strncpy( statmpath, "/proc/", strlen("/proc/") + 1 );
        strncat( statmpath, dir_struct->d_name, strlen(dir_struct->d_name) + 1 );
        
        if( lstat(path, &lstat_struct) != 0 )
        {
            printf("\nps: error in lstat function with %s in first argument: ", path );
            perror(NULL);
            exit(EXIT_FAILURE);
        }

        if( (lstat_struct.st_mode & S_IFMT) == S_IFDIR )
        {
            dp_inner = opendir(path);
            if(dp_inner == NULL)
            {
                printf("\nps: error in %s directory opening: ", path );
                perror(NULL);
                exit(EXIT_FAILURE);
            }




//                                                              CREATE "/proc/[pid]/stat" AND "/proc/statm"
            strncat( path, "/stat", strlen("/stat") + 1);
            strncat( statmpath, "/statm", strlen("/statm") + 1);
            
            fp = fopen( path, "r" );
            if( fp == NULL )
                continue;

            pass_struct = getpwuid( lstat_struct.st_uid );
            if( pass_struct == NULL )
            {
                printf("\nps: error in getwpuid function with %d in argument: ", lstat_struct.st_uid );
                perror(NULL);
                exit(EXIT_FAILURE);
            } 
            
            

//                                                              LOOK FOR 14th ELEMENT OF "/proc/[pid]/stat" - utime                      
            for( int index = 0; index < 13; index++ )
                while( getc(fp) != SPACE)   
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
                while( getc(fp) != SPACE)
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
                while( getc(fp) != SPACE)
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
                while( getc(fp) != ' ')
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