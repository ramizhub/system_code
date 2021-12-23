    #include <stdio.h>                                      
    #include <string.h>                                     // strlen()
    #include <unistd.h>                                     // read()


int main(int argc, char *argv[])                            // 3    ./a.out users_word file_txt 
{
    
    if(argc != 3)                                           // argCount should be three. the condition is not met - output usage
    {  
      printf("1Usage: %s string filename", argv[0]);        
      return 1;                                             // termination
    }

    
    FILE *fpointer = fopen(argv[2], "r");                   // get file pointer
   
    
    if(fpointer == NULL)                                    // null pointer returned if the program can't open file
    {
      perror("Could not open this file! Reason:  ");        // perror produces a message and describes the reasons
      return 1;                                             // termination
    }

      
    int n_word_size = strlen( argv[1] );                    // size of uzer's word
    int n_same_bytes = 0;                                   // same_bytes variable is 0 zero in the first iteration and when we find the wrong symbol
    int n_bytes_read;                                       // n_bytes_read works with read() and takes the number of bytes read in each while iteration
    int file_descriptor = fileno(fpointer);                 // first argument of read()
    
    
    char buffer[n_word_size];                               // size of buffer equals size of word because in normal while iteration we read n_word_size bytes
    char our_character;                                     // this variable is needed to memorize one single desired character and avoid reprocessing

    
    while( (n_bytes_read = read(file_descriptor, buffer, n_word_size)) != 0 )   // usually reads n_word_size bytes in buffer. max.size of buffer is n_word_size too
    {
      
      
      for(int index = 0; index < n_bytes_read; index++)     // for loop. every iteration here checks every byte of buffer. 
      {   
        
        if(n_same_bytes == n_word_size)                     // if the character is not the last in the checking of for_loop and the word is found, it will appear here
        {    
          printf("%5s\n", argv[1]);                         // print this word with new line character.
          our_character = '`';                              // give the character variable an unnecessary value
          n_same_bytes = 0;                                 // looking for a new word again thanks to this variable
        }

        
        else if(buffer[index] == our_character)             // character is needed character's copy and we must to skip it's checking
          continue;
        
            
        else if( buffer[index] == argv[1][n_same_bytes] )   // if current character is necessary character and it is not duplicate do these operations
        {  
          n_same_bytes++;                                   // this will make program check next character in argv[1][  ]  
          our_character = buffer[index];                    // give special character current
        }
           
           
        else if( buffer[index] != argv[1][n_same_bytes] )   // if we read unnecessary character we skip it and make program check argv[1][  ] from begining
        {
          n_same_bytes = 0;                                     
          our_character = '`';
        }
      
      }
      
      
      if(n_same_bytes == n_word_size)                       // if the character is the last in the checking of for_loop and the word is found, it will appear here
      {    
        printf("%5s\n", argv[1]);                           // print this word with new line character 
        our_character = '`';                                // give the character variable an unnecessary value
        n_same_bytes = 0;                                   // looking for a new word again thanks to this variable
      } 
    
    }


    if(fclose(fpointer) != 0)                               // if 0 returned it tells about error in closing                    
    { 
      perror("Could not close this one! Reason: ");         // perror produces a message and describes the reasons
      return 1;
    }


}