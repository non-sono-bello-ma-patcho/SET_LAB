#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<errno.h>
#include<string.h>
#include<fcntl.h>
#include <sys/stat.h>

void custom_fail(int *exit_state);

int main(int argc,char *argv [])
{
    if(argc!=3)
    {printf("USAGE:./autotester  {port} {number of test} \n");exit(EXIT_FAILURE);}
    else{printf("*********************INIZIO TEST*********************\n");}
    int n_test=atoi(argv[2]);
    int *pid=(int*)malloc(sizeof(int)*n_test),exit_state;
    int *status=(int*)malloc(sizeof(int)*n_test);
    int pipefd[2],fd;
    char message[]="\n****************************Eseguo richiesta telnet****************************\n";
    const char * commands[]={
      "HEAD / HTTP/1.0\r\n\r\n",
      "HEAD /aaa.html HTTP/1.0\r\n\r\n",
      "HEAAD / HTTP/1.0\r\n\r\n",
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaAAAAAAaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\r\n",
      "HEAD / HTTP/1.0\r\n\r\n",
      /*"GET /images/uncadunca.jpg HTTP/1.1\r\n\r\n",
      "GET /images/uncadunca.jpg HTTP/1.1\r\n\r\nIf-Modified-Since: Mon, 26 Nov 2019 11:27:51 GMT\r\n\r\n",*/
      NULL
    };
    
    
    /*controllo la grammatica*/
for(int i=0;commands[i]!=NULL;i++)
{
    fflush(stdout);
    if(pipe(pipefd)<0){perror("pipe failed:");exit(EXIT_FAILURE);}
    pid[0]=fork();
    if(pid[0]<0){perror("ERROR:fork failed");}
    if(pid[0]==0)
    {
        dup2(pipefd[0],STDIN_FILENO);
        close(pipefd[1]);close(pipefd[0]);
        exit_state=execl("/usr/bin/telnet","telnet","localhost",argv[1],(char*)NULL);    
        custom_fail(&exit_state);return 0;
    }
    else
    {
        close(pipefd[0]);
        sleep(2);
        write(2,message,strlen(message));
        if(write(pipefd[1],commands[i],strlen(commands[i]))<0){perror("fprintf failed:");exit(EXIT_FAILURE);}
        waitpid(pid[0],&status[0],0);
    }
    
}
system("clear");






printf("TEST TERMINATO!\n");
return 0;
}



void custom_fail(int *exit_state)
{
    if(exit_state<0){perror("ERRORE: ");exit(EXIT_FAILURE);}
    return;
}
