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
	char command[123], path[128];
	int pid, p1ex, p2ex, status;

	for(;;){
		printf("nano-bash $ ");
		if(fgets(command, 123, stdin)==NULL || strcmp("exit\n", command)==0) exit(EXIT_SUCCESS);
		if(command[strlen(command)-1]=='\n')command[strlen(command)-1]='\0'; /* delete \n char */
		
		memcpy(path, prefix, sizeof(prefix)); /* command = /bin/ */
		strcat(path, command); /* command = /bin/<command passed by user> */
		
		pid = fork();
		if(pid>=0){
			if(pid==0){
				p1ex = execl(path, path, NULL);
				if(p1ex<0){
					/* if command is not found in /bin/, search in/usr/bin/: */
					memcpy(path, prefix2, sizeof(prefix2)); /* command = /usr/bin/ */
					strcat(path, command); /* command = /usr/bin/<command passed by user> */
					p2ex = execl(path, path, NULL);
					if(p2ex <0)custom_perror("exec failed");
				}
			}
			waitpid(pid, &status, 0); /* wait fisrt child to end*/
		}
		else custom_perror("fork failed:");
	}
	return 42;
}
