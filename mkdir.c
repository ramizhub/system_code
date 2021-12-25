/*
    mkdir command receives 1 argument, which is the new directory to create.
    It exits 0 on success, and >0 if an error occurs. It does not print anything if the operation is successful.
*/

    #include <sys/stat.h>
    #include <sys/types.h>
    #include <stdio.h> 


int main(int argc, char *argv[])                                                // main function arguments
{
    if(argc != 2)                                                               // this_function_name users_directory_name
    {
        printf("Usage -> %s directory_name\n", argv[0]);                        // there i show user right usage 
        return 1;                                                               
    }
    
    if(mkdir( argv[1], 740 ) != 0 )               // users_directory_name and creation_mode 
    {
        perror("Error: ");                        
        return -1;                                // mkdir standard bad return 
    }
    
    return 0;                                     // mkdir standard success return
}