#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MAX_MD5 32
#define MAX_PATH 128
#define OFFSET 3
int pipe_read(int fd, char *buff);
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
//        FILE * fp=popen(command,"r");
//             if (fp == NULL) {
//            perror("popen");
//            exit(EXIT_FAILURE);
//        }
//        fgets(md5, MAX_MD5+strlen(path) + OFFSET, fp);
//        pclose(fp);
        int pipe_md5[2];
        pipe(pipe_md5);
        if(fork() == 0){
            close(pipe_md5[0]);

            dup2( pipe_md5[1],STDOUT_FILENO);
            close(pipe_md5[1]);

            char *args[] = {"/usr/bin/md5sum", path, NULL};
            execve(args[0], args, NULL);
            exit(EXIT_FAILURE);
        }
        pipe_read(pipe_md5[0], md5);
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

