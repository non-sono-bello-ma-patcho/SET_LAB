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
    if(argc!=4)
    {printf("USAGE:./autotester {webrootdir} {port} {number of test} \n");exit(EXIT_FAILURE);}
    else{printf("*********************INIZIO TEST*********************\n");}
    int n_test=atoi(argv[3]);
    int *pid=(int*)malloc(sizeof(int)*n_test),exit_state;
    int *status=(int*)malloc(sizeof(int)*n_test);
    int pipefd[2];
    char temp[]="GET /images/uncadunca.jpg HTTP/1.1\r\n\r\n\0";
    const char * commands[]={
      "HEAD / HTTP/1.0\r\n\r\n",
      "HEAD /aaa.html HTTP/1.0\r\n\r\n",
      "HEAAD / HTTP/1.0\r\n",
      "GET /images/uncadunca.jpg HTTP/1.1\r\n",
      "GET /images/uncadunca.jpg HTTP/1.1\r\n\r\nIf-Modified-Since: Mon, 26 Nov 2014 11:27:51 GMT\r\n"
    };
    
    
    /*controllo la grammatica*/
    if(pipe(pipefd)<0){perror("pipe failed:");exit(EXIT_FAILURE);}
    pid[0]=fork();
    if(pid[0]<0){perror("ERROR:fork failed");}
    if(pid[0]==0)
    {
        dup2(pipefd[0],STDIN_FILENO);
        close(pipefd[1]);close(pipefd[0]);
        exit_state=execl("/usr/bin/telnet","telnet","localhost",argv[2],(char*)NULL);    
        custom_fail(&exit_state);return 0;
    }
    else
    {
        close(pipefd[0]);
        sleep(3);
        perror("\nEseguo richiesta telnet:\n");
        if(write(pipefd[1],temp,strlen(temp))<0){perror("fprintf failed:");exit(EXIT_FAILURE);}
        perror("aspetto child \n");
        waitpid(pid[0],&status[0],0);
    
    }













   /*
    for(int i=0;i<n_test;i++)
    {
        if(pipe(pipefd)<0){perror("pipe failed:");exit(EXIT_FAILURE);}
        pid[i]=fork();
        if(pid[i]<0){perror("ERROR:fork failed");}
        if(pid[i]==0)
        {
            dup2(pipefd[0], 0);
            close(pipefd[0]);
        
            printf("\nEseguo richiesta[%d]:\n",i+1);
            exit_state_client=execl("/usr/bin/telnet","telnet","localhost",argv[2],(char*)NULL);    
            custom_fail(&exit_state);return 0;
        }
        else
        {
            dup2(pipefd[1], 1); 
            close(pipefd[1]);
        }
    }
    
    for(int i=0;i<l_str_test;i++)
    {
        if(write(stdout,command[i])<0){{perror("write failed:");exit(EXIT_FAILURE);}
    
    
    
    }

    for(int i=0;i<n_test;i++)
    {
        waitpid(pid[i],&status[i],0);}
    }*/

printf("TEST TERMINATO!\n");
return 0;
}



void custom_fail(int *exit_state)
{
    if(exit_state<0){perror("ERRORE: ");exit(EXIT_FAILURE);}
    return;
}
