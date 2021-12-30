/*
    This is my personal little library with the functions that I will use in this repository. 
    I add here those functions, the originals of which did not suit me.
*/
    #include <unistd.h>


                                                                                
ssize_t r_read(int file_descriptor, char * user_string, int bytes_to_read)      // read imitation function, that adds '\n' aftest last readed character
{                                                     
    ssize_t readed_BYTES;                                                       // that is function return value
    
    readed_BYTES = read(file_descriptor, user_string, (bytes_to_read - 1));     // bytes_to_read - 1 => reason => i need guaranteed place for '\0'
    if(readed_BYTES == 0)                                                       // just want to draw attention on this
        return 0;
    else
        user_string[readed_BYTES] = '\0';                                       // setting my character after array's last element                                     
     
    return readed_BYTES;                                                        // returning readed bytes count like original read function does
}