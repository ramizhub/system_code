#include <stdio.h> 
#include <unistd.h>


int main() {

    char users_file_name[256]; // 256 characters massiv
    char buffer[600];
    ssize_t n_bytes_read;
    ssize_t summa = 0;
    
    

    printf("the name of your file: "); // i ask user the name of his file
    scanf("%s", users_file_name);


    printf("%s! process was ended ", users_file_name); // there would be name of his file and sucesfull/unsucesfull
    // FILE *fopen(const char *restrict pathname, const char *restrict mode);
        
    FILE *fp = fopen( users_file_name ,  "r"); // created file pointer *fp
    
    long size;
 
    

    // fopen returns file pointer if there were no problems
     
     
    // NULL is nothing and fopen returns pointer to NULL when there is a problem
    
     
    if(fp == NULL)        // fp - file pointer
    printf("  unsucesfull     \n ");
    else 
    printf("     sucesfull    \n ");
    
    int filedes;
    filedes = fileno(fp); // filedes accepted fileno return vallue  fileno -> filedes 
    printf("Ur file descriptor:        %d\n", filedes); // filedes - file descriptor
    

    if( (n_bytes_read = read(filedes, buffer, 600)) < 600 )
    {
    printf("\n%s\n", buffer);  
    printf("Readed bytes count : %ld", n_bytes_read);
    }

    else
    {
    while( (n_bytes_read = read(filedes, buffer, 600)) == 600 )
    {
    printf("%s", buffer);
    summa = summa + n_bytes_read;
    }
    if(n_bytes_read < 600)
    summa = summa + n_bytes_read;
    
    printf("\n");
    printf("Readed bytes count: ");
    printf("%ld\n", summa);
    }
    
    
    if (fclose(fp) == 0) 
    printf("\nfile closed\n");
    else
    printf("closing error\n");


   // after report on opening the program there would be report on closing
       return 0;
}



// int fileno(FILE *stream) returns file descriptor and file descriptor is always a positive number 
// in case of error he will return -1
