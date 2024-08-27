#include "commons.h"
#include "pipe_master.h"
#define MAX_MD5 32
#define MAX_PATH 128
#define OFFSET 3
int main (int argc, const char * argv[]){

    char path[MAX_PATH] ;
    char *md5_cmd="md5sum %s";
    
    char command[MAX_PATH + strlen(md5_cmd)];
    while(pipe_read(STDIN_FILENO, path)>0){
          if (path[0] == 0) {
            // No more files to read, exit the loop
            break;
        }
        char md5[MAX_MD5+strlen(path)+OFFSET];
        sprintf(command,md5_cmd,path);
        FILE * fp=popen(command,"r");
             if (fp == NULL) {
            perror("popen");
            exit(EXIT_FAILURE);
        }
        fgets(md5, MAX_MD5+strlen(path)+ OFFSET, fp);
        pclose(fp);
        write(STDOUT_FILENO,md5,strlen(md5)+1);
    }
    close(STDOUT_FILENO);
    exit(EXIT_SUCCESS);
 
 
 
 
 //write(STDOUT_FILENO, "buff", strlen("buff")+1);

// hacer read, tener en cuenta que puede venir mas de un file a la vez
// hacer chequeo de que se leyo algo
// manejar buffer de lectura, no agarrar de a un caracter



// llamar a md5 (investigar el uso de system || o popen() )
// enviar nombre y md5 al padre (ver como)
}
