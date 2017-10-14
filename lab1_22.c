#include <stdlib.h>
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

int main(){
	char prefix[]="/bin/";
	char prefix2[]="/usr/bin/";
	char option[123], command[128];
	int pid, p1ex, p2ex, status;

	for(;;){
		printf("nano-bash $ ");
		if(fgets(option, 123, stdin)==NULL || strcmp("exit\n", option)==0) exit(EXIT_SUCCESS);
		option[strlen(option)-1]='\0'; /* delete \n char */
		memcpy(command, prefix, strlen(prefix));
		strcat(command, option);
		pid = fork();
		if(pid>=0){
			if(pid==0){
				p1ex = execl(command, command, NULL);
				if(p1ex<0){
					memcpy(command, prefix2, strlen(prefix2));
					strcat(command, option);
					p2ex = execl(command, command, NULL);
					if(p2ex <0)custom_perror("exec failed");
				}
			}
			waitpid(pid, &status, 0); /* wait fisrt child to end*/
		}
		else custom_perror("fork failed:");
	}
	return 42;
}
