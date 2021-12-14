#include <stdio.h> 
#include <unistd.h>


int main(int argc, char *argv[])               // (int) argument count.   (char *) argv[] argument values
{    

  if(argc != 2)                                // in this program i need 2 arguments in command line: function's and file's names.
    { 
       printf("Please, enter the name of function: %s  and after the space button print ur filename", argv[1] ); 
       return 1;                                // evacuation from my program
    }
  
  

    char buffer[601];                          // element with 600 index will be '\0'
    ssize_t n_bytes_read;
    ssize_t summa = 0;
    

    printf("%s is opening. Waiting ...\n", argv[1]);  
    
        
    FILE *fp = fopen( argv[1] ,  "r");         // created file pointer *fp
    
    // FILE *fopen(const char *restrict pathname, const char *restrict mode);
 
    // fopen returns file pointer if there were no problems
         
    // NULL is nothing and fopen returns pointer to NULL when there is a problem
    
     
    if(fp == NULL)                             // fp - file pointer
    {                
       printf("I can't open this file...\n");
       perror("Reason: ");
       return 1;
    }
    else 
    {
       printf("\nthe opening was successful\n");
    
    
    int filedes;
    filedes = fileno(fp);                      // filedes accepted fileno return vallue  fileno -> filedes 
    printf("Ur file descriptor:        %d\n", filedes); // filedes - file descriptor
    
    
    
    while( (n_bytes_read = read(filedes, buffer, 600)) != 0 )
    
    {
       summa = summa + n_bytes_read; 
       buffer[n_bytes_read] = '\0';
       printf("%s", buffer);                    // printf displays everything before NULL
    }
    
    printf("\nReaded bytes count:  %ld\n", summa); 


    if (fclose(fp) == 0) 
       printf("File closed sucesfully.");
    else
       {
         printf("File cant be closed.\n");
         return 1;
       }


   // after report on opening the program there would be report on closing
       return 0;

    }
    
}



   // int fileno(FILE *stream) returns file descriptor and file descriptor is always a positive number 
   // in case of error he will return -1
