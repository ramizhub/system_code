    #include <stdio.h>                                                          // printf function
    #include <string.h>                                                         // strlen function
    
    #include <time.h>                                                           // ctime function
    
    #include <dirent.h>                                                         // working with directory
    #include <sys/types.h>                                                      // data types
    #include <sys/stat.h>                                                       // data returned by the stat() function
    #include <pwd.h>                                                            // password structure
    #include <grp.h>

int main(int argc, char * argv[])                                               // argc = 2, compiled_program directory_name
{
    if(argc != 2)
    {
        puts("Usage == >  compiled_program directory_name");                    // show conditions of usage
        return 1;
    }
    
    DIR * directory_pointer = opendir( argv[1] );                               // creating directory pointer and opening user's directory
    
    if(directory_pointer == NULL)                                               // NULL returned on error, show number of error in this situation
    {
        perror("Can't open this directory. Reason == >  ");
        return 1;
    }
 
    struct stat object_statistic;                                               // structure, which contains data about every file ( would contain )
    struct dirent *directory_statistic;                                         // structure, from which i would take name of every file
    struct passwd *user_id_statistic;                                           // structure, which contains data about passwords and important user_name
    struct group  *group_id_statistic;                                          // structure, which contains data about groups


    char * string_pointer;                                                      // pointer to string, which ctime function would return
    
    int files_count = 0;                                                        // total == > files count 
    size_t helpful_size;                                                        // i need to replace new line character by null character. 
    
  
    while( (directory_statistic = readdir(directory_pointer))  != NULL  )       // in the end of file or on error NULL returned
    {
        files_count++;               
        stat(directory_statistic->d_name, &object_statistic);                  
        // take name of current file, fill this structure with information about this particular file

        string_pointer = ctime( &object_statistic.st_mtime);                    
        helpful_size = strlen(string_pointer);                                  // replacing new line character by null character
        string_pointer[helpful_size - 1] = '\0';                                // last one is '\0'. penultimate - \n. \n == > \0
        
        switch (object_statistic.st_mode & S_IFMT)                              // asking st_mode
        {
            case S_IFREG:                                                       // is it a regular file? put '-'
                putchar('-');
                break;
            case S_IFDIR:                                                       // is it a directory? put 'd'
                putchar('d');
                break;
            case S_IFBLK:                                                       // is it a block device? put 'b'
                putchar('b');
                break;
            case S_IFCHR:                                                       // is it a character device? put 'c'
                putchar('c');
                break;
            case S_IFLNK:                                                       // is it a symbolic link? put 'l'
                putchar('l');
                break;
            case S_IFSOCK:                                                      // is it a socket? put 's'
                putchar('s');
                break;
            default:                                                            // if an error occured
                putchar('?');
                break;                     
        }
        
        
        if(object_statistic.st_mode & S_IRUSR)
            putchar('r');
        else
            putchar('-');
           
        if(object_statistic.st_mode & S_IWUSR)
            putchar('w');
        else
            putchar('-');

        if(object_statistic.st_mode & S_IXUSR)
            putchar('x');
        else
            putchar('-');

        if(object_statistic.st_mode & S_IRGRP)
            putchar('r');
        else
            putchar('-');

        if(object_statistic.st_mode & S_IWGRP)
            putchar('w');
        else
            putchar('-');                                                       // asking st_mode: Can this read? Or write? Or execute?
   
        if(object_statistic.st_mode & S_IXGRP)
            putchar('x');
        else
            putchar('-');

        if(object_statistic.st_mode & S_IROTH)
            putchar('r');
        else
            putchar('-');    

        if(object_statistic.st_mode & S_IWOTH)
            putchar('w');
        else
            putchar('-');     

        if(object_statistic.st_mode & S_IXOTH)
            putchar('x');
        else
            putchar('-');     
        
        
        user_id_statistic = getpwuid(object_statistic.st_uid);                  // return pointer to a structure with user's name
        
        printf("\t%s", user_id_statistic->pw_name);

        group_id_statistic = getgrgid(object_statistic.st_gid);                 // return pointer to a structure with group's name

        printf("\t%s", group_id_statistic->gr_name);
    
        printf("\t%ld", object_statistic.st_size);                              // show user information about current file
    
        printf("\t%s", string_pointer);

        printf("\t%s", directory_statistic->d_name);
        
        putchar('\n');                                                          // start printing information about new file with new line character
    } 
    
    printf("total - %d", files_count);                                     
    putchar('\n');

    if( (closedir(directory_pointer)) != 0)                                     // closing directory and looking for errors
    {
        perror("Can't close this directory. Reason == >  ");
        return 1;
    }

    return 0;
}