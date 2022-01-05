    #include <stdio.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
 
    #define ARRAY_SIZE 40



//            ./a.out source_file target_file
int main(int argc, char * argv[])
{
    FILE * file_pointer_1 = fopen(argv[1], "r");
    if(file_pointer_1 == NULL)
    {
        perror("error ;/  reason == >    ");
        return 1;
    }
   
    FILE * file_pointer_2 = fopen(argv[2], "w");
    
    char reading_buffer[ARRAY_SIZE];
    while( fgets(reading_buffer, ARRAY_SIZE, file_pointer_1) != NULL)
        fputs(reading_buffer, file_pointer_2);
    
    
    if( fclose(file_pointer_1) != 0   ||   fclose(file_pointer_2) != 0    )
    {
        perror("error ;/  reason == >    ");
        return 1;
    }



    return 0;
}    