/*
    mkdir command receives 1 argument, which is the new directory to create.
    It exits 0 on success, and >0 if an error occurs. It does not print anything if the operation is successful.
*/

    #include <sys/stat.h>                                             // mkdir function                                             
    #include <sys/types.h>                                            // mkdir function => mode_t           
    #include <stdio.h>                                                // printf                                               
    #include <string.h>                                               // strlen function
    #include <stdlib.h>

int main(int argc, char *argv[])                                      // usage!  ./a.out directory_name directory_mode ( octal )                            
{
    
    if(argc != 3)                                                               
    {
        printf("Usage -> %s directory_name mode\n", argv[0]);         // printing usage for user               
        return 1;                                                               
    }
    
    


    int second_int_argument = 0, power_eight_value, our_int_array_element;  
    // second_int_argument == > our second mkdir function argument, which means creating mode. that is int digit
    // power_eight_value == > powered eight, which changes own value in every iteration depending on the order
    // our_int_array_element == > that is int digit too, i convert octal to i and save value in this variable   
    

    // element_index == > that is argv array elements' index. we have two responsible for elements' index parallel working variables, second is power
    // power is responsible for eight's degree
   
   
    for( int element_index = 0, power = strlen(argv[2]) - 1; element_index < strlen(argv[2]); element_index++, power--)
    {
        power_eight_value = 1;                                                  // preparing this variable for multiplication
        for(int power_times = 1; power_times <= power; power_times++)           // responsible for powered eight loop
            power_eight_value *= 8;
        
        our_int_array_element = (int) argv[2][element_index] - 48;              // casting octal string to integer digit
        second_int_argument += our_int_array_element * power_eight_value;       // sum all values and save it in second_int_argument which would be mkdir argument
    }
    
    
    if(mkdir( argv[1], second_int_argument ) != 0 )                             // creating directory and showing bugs with perror              
    {
        perror("Error: ");                        
        return -1;                                                           
    }
    
    return 0;                                                                 
}