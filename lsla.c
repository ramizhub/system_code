    #include <stdio.h>                                                          
    #include <string.h>                                                         
    
    #include <time.h>                                                                     // ctime function
    
    #include <dirent.h>                                                         
    #include <sys/types.h>                                                      
    #include <sys/stat.h>                                                                 // for working with system data                                  
    #include <pwd.h>                                                            
    #include <grp.h>

int main(int argc, char * argv[])                                                         // 2 arguments
{
    if(argc != 2)
    {
        printf("usage: compiled_program file  or  compiled_program directory");
        return 1;
    }
 
    struct stat object_stat;                                                              // general structure ( for both of cases )
    if( stat(argv[1], &object_stat) != 0)
    {
        perror("ls: stat error: ");
        return 1;
    } 
    
    if( (object_stat.st_mode & S_IFMT) == S_IFREG )                                       // working with single file 
    {
        struct passwd * pass_stat;
        struct group * group_stat;
        
        char * string_pointer;
        
        
        stat(argv[1], &object_stat);
        pass_stat = getpwuid(object_stat.st_uid);                                         // pullin information for structures
        group_stat = getgrgid(object_stat.st_gid);

        string_pointer = ctime( &object_stat.st_mtime);                                   
        string_pointer[strlen(string_pointer) - 1] = '\0';                                // executing new line character from returned string

        putchar('-');                                                                     // anyway that is file
        
        
        printf( (object_stat.st_mode & S_IRUSR) ? "r" : "-");                 
        printf( (object_stat.st_mode & S_IWUSR) ? "w" : "-");
        printf( (object_stat.st_mode & S_IXUSR) ? "x" : "-");
        
        printf( (object_stat.st_mode & S_IRGRP) ? "r" : "-");
        printf( (object_stat.st_mode & S_IWGRP) ? "w" : "-");                             // showing modes
        printf( (object_stat.st_mode & S_IXGRP) ? "x" : "-");
        
        printf( (object_stat.st_mode & S_IROTH) ? "r" : "-");
        printf( (object_stat.st_mode & S_IWOTH) ? "w" : "-");
        printf( (object_stat.st_mode & S_IXOTH) ? "x" : "-"); 

        
        printf("\t%s", pass_stat->pw_name);
        printf("\t%s", group_stat->gr_name);

        printf("\t%ld", object_stat.st_size);

        printf("\t%s", string_pointer);

        printf("\t%s", argv[1]);
 
        putchar('\n');

    }
    
    else if( (object_stat.st_mode & S_IFMT) == S_IFDIR)                                   // working with directory
    {
        DIR * dir_pointer = opendir( argv[1] ); 
        if(dir_pointer == NULL)                                                           // problem with opening of directory
        {
            perror("ls: can't open this directory: ");
            return 1;
        } 

        struct dirent * dir_stat;
        struct passwd * pass_stat;
        struct group * group_stat;

        char * string_pointer;

        int files_count = 0;                                                              // my version of total -

        while( (dir_stat = readdir(dir_pointer))  != NULL  )                              // opening directory
        {
            files_count++;

            stat(dir_stat->d_name, &object_stat);
            pass_stat = getpwuid(object_stat.st_uid);                                     // pulling information for structures
            group_stat = getgrgid(object_stat.st_gid);  
            
            switch (object_stat.st_mode & S_IFMT)                                         // looking for types of files
            {
                case S_IFREG:                                                       
                putchar('-');
                break;
                
                case S_IFDIR:                                                       
                putchar('d');
                break;
                
                case S_IFBLK:                                                       
                putchar('b');
                break;
                
                case S_IFCHR:                                                       
                putchar('c');
                break;
                
                case S_IFLNK:                                                       
                putchar('l');
                break;
                
                case S_IFSOCK:                                                      
                putchar('s');
                break;
                
                default:                                                            
                putchar('?');
                break;                     
            }
            
            
            printf( (object_stat.st_mode & S_IRUSR) ? "r" : "-");
            printf( (object_stat.st_mode & S_IWUSR) ? "w" : "-");
            printf( (object_stat.st_mode & S_IXUSR) ? "x" : "-");
        
            printf( (object_stat.st_mode & S_IRGRP) ? "r" : "-");
            printf( (object_stat.st_mode & S_IWGRP) ? "w" : "-");                         // showing modes
            printf( (object_stat.st_mode & S_IXGRP) ? "x" : "-");
        
            printf( (object_stat.st_mode & S_IROTH) ? "r" : "-");
            printf( (object_stat.st_mode & S_IWOTH) ? "w" : "-");
            printf( (object_stat.st_mode & S_IXOTH) ? "x" : "-"); 

            
            string_pointer = ctime(&object_stat.st_mtime);
            string_pointer[strlen(string_pointer) - 1] = '\0';                            // executing new line character from returned string

            printf("\t%s", pass_stat->pw_name); 
            printf("\t%s", group_stat->gr_name);

            printf("\t%ld", object_stat.st_size);

            printf("\t%s", string_pointer);

            printf("\t%s", dir_stat->d_name);

            putchar('\n');

        }
        
        printf("total - %d", files_count);
        putchar('\n');

        if( closedir(dir_pointer) != 0)                                                   
        {
            perror("ls: can't close this directory: ");
            return 1;
        }
    }
    return 0;                                                                             // positive case
}