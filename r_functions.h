#include <unistd.h>

ssize_t r_read(int file_descriptor, char * user_string, int bytes_to_read)
{
    ssize_t readed_BYTES;
    
    readed_BYTES = read(file_descriptor, user_string, (bytes_to_read - 1));
    if(readed_BYTES == 0)
        return 0;
    else
        user_string[readed_BYTES] = '\0';                
    
    return readed_BYTES; 
}