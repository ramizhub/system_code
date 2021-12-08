#include <stdio.h> // понадобится могу ввести и другую библиотеку но этой хватило
#include <unistd.h>


int main() {
char users_file_name[256]; // создал массив данных из 256 символов
char buffer[600];
ssize_t n_bytes_read;



printf("the name of your file: "); // прошу пользователя назвать свой файл чтобы программка открыла его
scanf("%s", &users_file_name);


      printf("%s! process was ended ", users_file_name); // тут я впишу название файлика и рядом с ним будет отчет об успехе или провале
      // FILE *fopen(const char *restrict pathname, const char *restrict mode);
        
    FILE *fp = fopen( users_file_name ,  "r"); // создал указатель и открыл файл
    
    long size;
 
    

/* так как функция fopen при успехе программки вернет мне указатель, а при неуспешности выполнения 
вернет NULL, я указал в условии именно это */
  /* этот NUll переводится как ничто, при ошибках с нахождением файла он указывает на ничто, ведь
  он обязан отправить нам указатель. */
     if(fp == NULL)    // fp - указатель на файл.
       printf("  unsucesfull     \n ");
     else 
      printf("     sucesfull    \n ");
    int filedes;
    filedes = fileno(fp); // одновременно приравнял выход функции fileno к filedes и запустил функцию, которая вернет файловый дескриптор.
    printf("Ur file descriptor:        %d\n", filedes); // filedes - файловый дескриптор.
  
   n_bytes_read = read(filedes, buffer, 600);
    
    if(n_bytes_read == -1)
     printf("cannot read this file.\n");
    else if(n_bytes_read < 600) 
     printf("not all bytes were read\n");
    else  
     printf("file readed sucesfully\n");
      
    
      printf(" Readed bytes count :        %d\n", n_bytes_read);

       printf("%s\n", buffer);        // распечатал содержимое буфера



     if (fclose(fp) == 0) /* моей ошибкой до этого было то, что я приравнивал сам указатель,
     а приравнивать нужно всю функцию вместе с указателем в аргументе и тогда все заработает корректно */
      printf("\nfile closed\n");
    else
     printf("closing error\n");



   // рядом с отчетом об успехе или неудачи при открытии файла сразу будет отчет о закрытии  
     
  

   return 0;
}



// int fileno(FILE *stream) возвращает нам файловый дескриптор и этот файловый дескриптор всегда положительное число 
// эта функция в случае неудачи вернет нам -1.
