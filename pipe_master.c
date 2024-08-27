#include "commons.h"
#include "pipe_master.h"


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