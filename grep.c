#include <stdio.h>
#include <string.h>                    // strlen
#include <unistd.h>

int main(int argc, char *argv[])       // two main function arguments
{
   if(argc != 3)                       // argument count must be 3 because first is function name, second is searched word, third is file's name
   {  
     printf("1Usage: %s string filename", argv[0]);        // if there is less or more than 3 arguments i will show my usage conditions
     return 1;                                             // return looks like break in loops
   }

   FILE *fpointer = fopen(argv[2], "r");                   // open file and creatin stdin
   
   
   if(fpointer == NULL)                                    
   {
     perror("Could not open this file! Reason:  ");        // perror looks for the last error and shows it's number after my printed string
     return 1;
   }
   
    
   
   int n_word_size = strlen( argv[1] );                    // that is word's length
   int n_same_bytes = 0;                                   // same bytes count and that is zero because i need this zero at the begining
   int n_bytes_read;                                       // i need to know for until which number loop should work
   int file_descriptor = fileno(fpointer);                 // read's argument file descriptor
   
   char buffer[n_word_size];                               // i read from txt file as many bytes as characters in word

   while( (n_bytes_read = read(file_descriptor, buffer, n_word_size)) != 0 )       // if read returned zero this is file's end
   {
       for(int index = 0; index < n_bytes_read; index++)   // the second sentence is so important i need to read exactly as much as readed read function (n_bytes_read - 1)
       {
         if(  buffer[index] == argv[1][n_same_bytes]  )    // comparing the characters of the buffer and a two-dimensional array of the user     
           n_same_bytes++;                                 // this one help me to know which element in argv[1] is already checked and which i need to try in the next loop
                
         else if(n_same_bytes == n_word_size )             // means that the number of characters you want is actually present
         {  
           printf("%s\n", argv[1]);                        // print user's word
           n_same_bytes = 0;                               // after finding word i think that this one is right 
         }
 
         else if(buffer[index] != argv[1][n_same_bytes])   // if at least one character in row is not that i need i need to refresh this because word is specific row
           n_same_bytes = 0;



       }
   }
   
   if(fclose(fpointer) != 0)                               // closing user's file and if 0 not returned error is here and perror will show it's number         
   { 
     perror("Could not close this one! Reason: ");        
     return 1;
   }


}