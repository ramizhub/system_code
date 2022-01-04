    #include <sys/types.h>
    #include <sys/stat.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <time.h>
    


int main(int argc, char * argv[])
{
    if(argc != 2)
    {
        puts("Usage == >compiled_program file_name");
        return 1;
    }
    
    struct stat object_statistic;
    
    stat(argv[1], &object_statistic);
    printf("Total %d\n", argc - 1);
    
    printf("Working mode - %lo \n", (unsigned long) object_statistic.st_mode);
    
    printf("Owner id - %d \n", object_statistic.st_uid);
    
    printf("Group id - %d \n", object_statistic.st_gid);
    
    printf("File size - %ld \n", object_statistic.st_size);
    
    printf("Last modification time - %s", ctime(&object_statistic.st_mtime));

    return 0;
}