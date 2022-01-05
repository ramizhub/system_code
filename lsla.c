    #include <stdio.h>                                                // printf function
    #include <string.h>                                               // strlen function
    
    #include <time.h>                                                 // ctime function
    
    #include <dirent.h>                                               // working with directory
    #include <sys/types.h>                                            // data types
    #include <sys/stat.h>                                             // data returned by the stat() function
    #include <pwd.h>                                                  // password structure


int main(int argc, char * argv[])                                     // argc = 2, compiled_program directory_name
{
    if(argc != 2)
    {
        puts("Usage == >  compiled_program directory_name");          // show conditions of usage
        return 1;
    }
    
    DIR * directory_pointer = opendir( argv[1] );                     // creating directory pointer and opening user's directory
    
    if(directory_pointer == NULL)                                     // NULL returned on error, show number of error in this situation
    {
        perror("Can't open this directory. Reason == >  ");
        return 1;
    }
 
    struct stat object_statistic;                                     // structure, which contains data about every file ( would contain )
    struct dirent *directory_statistic;                               // structure, from which i would take name of every file
    struct passwd *user_id_statistic;                                 // structure, which contains data about passwords and important user_name ( user or group )

    char * helpful_pointer;                                           // pointer to string, which ctime function would return
    
    int files_count = 0;                                              // total == > files count 
    size_t helpful_size;                                              // i need to replace new line character by null character. 
    
    printf("mode      u_name      g_name      f_size         lmod_time              name");
    putchar('\n');

    while( (directory_statistic = readdir(directory_pointer))  != NULL  )       // in the end of file or on error NULL returned
    {
        files_count++;               
        stat(directory_statistic->d_name, &object_statistic);                  
        // take name of current file, fill this structure with information about this particular file

        helpful_pointer = ctime( &object_statistic.st_mtime);                    
        helpful_size = strlen(helpful_pointer);                                 // replacing new line character by null character
        helpful_pointer[helpful_size - 1] = '\0';                               // last one is '\0'. penultimate - \n. \n == > \0
        
        printf("%lo     ", (unsigned long) object_statistic.st_mode);
    
        user_id_statistic = getpwuid(object_statistic.st_uid);                  // return pointer to a structure with users' name
        
        printf("%s      ", user_id_statistic->pw_name);

        user_id_statistic = getpwuid(object_statistic.st_gid);
    
        printf("%s      ", user_id_statistic->pw_name);
    
        printf("%ld     ", object_statistic.st_size);                           // show user information about current file
    
        printf("%s      ", helpful_pointer);

        printf("%s      ", directory_statistic->d_name);
        
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