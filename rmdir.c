/*
    rmdir command receives 1 argument, which is the EMPTY directory to remove.
    It exits 0 on success, and >0 if an error occurs. It does not print anything if the operation is successful.
*/

    #include <unistd.h>                                         // rmdir function 
    #include <stdio.h>                                          



int main(int argc, char *argv[])                                // main function arguments
{
    if(argc != 2)                                               // this_function_name users_directory_name
    {
        printf("Usage -> %s directory_name\n", argv[0]);        // there i show user right usage
        return 1;                                               
    }    
   
    if(  rmdir(argv[1]) != 0 )                                  // users_directory_name
    {
        perror("Error: ");                                 
        return -1;                                              // rmdir standard bad return
    }
    
    return 0;                                                   // rmdir standard good return
}