#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
int pipe_read(int fd, char *buff);
int main (int argc, const char * argv[]){

    char path[100] ;
    while(pipe_read(STDIN_FILENO, path)>0){
          if (path[0] == 0) {
            // No more files to read, exit the loop
            break;
        }
        write(STDOUT_FILENO,"hola",strlen("hola")+1);
    }
    exit(EXIT_SUCCESS);
 
 
 
 
 //write(STDOUT_FILENO, "buff", strlen("buff")+1);

// hacer read, tener en cuenta que puede venir mas de un file a la vez
// hacer chequeo de que se leyo algo
// manejar buffer de lectura, no agarrar de a un caracter



// llamar a md5 (investigar el uso de system || o popen() )
// enviar nombre y md5 al padre (ver como)
}
int pipe_read(int fd, char *buff){
    int i=0;
    char last_charater_read[1];
    last_charater_read[0]=1;

    while(last_charater_read[0]!=0 && last_charater_read[0]!='\n' && read(fd,last_charater_read,1)>0){
        buff[i++]=last_charater_read[0];
    }
    buff[i]=0;

    return i;
}

