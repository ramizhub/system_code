/*
    mkdir command receives 1 argument, which is the new directory to create.
    It exits 0 on success, and >0 if an error occurs. It does not print anything if the operation is successful.
*/

    #include <sys/stat.h>                                                       
    #include <sys/types.h>                                                      
    #include <stdio.h> 
    #include <stdlib.h>

int main(int argc, char *argv[])                                                
{
    
    if(argc != 3)                                                               
    {
        printf("Usage -> %s directory_name mode\n", argv[0]);                       
        return 1;                                                               
    }
   
    
    if( atoi(argv[2]) > 7777 || atoi(argv[2]) < 1 )
    {
        puts("Check acceptable values");
        return 1;
    }
  

    if(mkdir( argv[1], *argv[2] ) != 0 )                    
    {
        perror("Error: ");                        
        return -1;                                                           
    }
    
    return 0;                                                                 
}