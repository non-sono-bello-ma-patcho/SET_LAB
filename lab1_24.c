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
	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]){
	int pid, pipefd[2], status, execres; /* create a new process */
	char command1[] = "/bin/ps";
	char command2[]="/bin/grep";

	/* create tunnel with pipe */
	if(pipe(pipefd)<0) custom_perror("pipe failed:");

	pid = fork();
	if(pid<0) custom_perror("fork failed");
	if(pid==0){
		/* child process */
		dup2(pipefd[0], 0); /* now p[0] contains 0 */
		close(pipefd[0]);
		/* execute the ps command */
		execres=execl(command2, command2, "bash", NULL);
		if(execres<0) custom_perror("execl failed");
	}
	else{
		/* parent process */
		dup2(pipefd[1], 1); /* now contains 1 */
		close(pipefd[1]);
		execres=execl(command1, command1, "aux", NULL);
		if(execres<0) custom_perror("execl failed");		
	}
	waitpid(pid, &status, 0);
	exit(EXIT_SUCCESS);
}