#include <stdio.h> 
#include <unistd.h>


int main(int argc, char *argv[])               // (int) argument count.   (char *) argv[] argument values
{    

  if(argc != 2)                                // in this program i need 2 arguments in command line: function's and file's names.
    { 
       printf("Usage: %s file_name_to_read\n", argv[0] ); 
       return 1;                                // evacuation from my program
    }
  
  

    char buffer[601];                          // element with 600 index will be '\0'
    ssize_t n_bytes_read;
    
    

    printf("%s is opening. Waiting ...\n", argv[1]);  
    
        
    FILE *fp = fopen( argv[1] ,  "r");         // created file pointer *fp
    
    // FILE *fopen(const char *restrict pathname, const char *restrict mode);
 
    // fopen returns file pointer if there were no problems
         
    // NULL is nothing and fopen returns pointer to NULL when there is a problem
    
     
    if(fp == NULL)                             // fp - file pointer
    {                
       puts("I can't open this one...");
       puts("Reason: ");
       return 1;
    }
    puts("Opening ended succesfully");
    
    
    int filedes;
    filedes = fileno(fp);                      // filedes accepted fileno return vallue  fileno -> filedes 
    printf("Ur file descriptor: %d\n", filedes); // filedes - file descriptor
    
    
    
    while( (n_bytes_read = read(filedes, buffer, 600)) != 0 )
    
    { 
       buffer[n_bytes_read] = '\0';
       printf("%s", buffer);                    // printf displays everything before NULL
    }
    
     

    if(fclose(fp) == EOF)
    {
      perror("Could not close file! Reason: ");
      return 1;
    }
    putchar('\n');
    puts("File was closed succesfully");  
    
    // after report on opening the program there would be report on closing
    return 0;

    
}



   // int fileno(FILE *stream) returns file descriptor and file descriptor is always a positive number 
   // in case of error he will return -1
