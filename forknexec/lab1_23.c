#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

void custom_perror(char* line){
	perror(line);
	exit(1);
}

int main(int argc, char* argv[]){
	int pid = fork(), execres, nfd, status; /* create a new process */
	char command[] = "/bin/ls";
	char option[]="-l";
	if(pid<0) custom_perror("fork failed");
	if(pid==0){
		nfd=open(argv[1], O_CREAT | O_RDWR);
		dup2(nfd, 1);
		close(nfd);
		execres=execl(command, command, option, NULL);
		if(execres<0) custom_perror("execl failed");
	}
	waitpid(pid, &status, 0);
	return 42;
}
