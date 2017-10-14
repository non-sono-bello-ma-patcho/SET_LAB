# Another crappy repository!
This repository contains all my project for the *Trasmission and Processing System* class I'm currently following!

## Project's list:
1. fork and exec:
   - The [lab1-22](SET_LAB/lab1_22.c):
     - waits for a command name to be entered(to get user's input I used the [fgets(3)](http://man7.org/linux/man-pages/man3/gets.3.html) system call );
     - if `exit` or `^D` are entered, exits using [exit(3)](http://man7.org/linux/man-pages/man3/exit.3.html) with *exit status* `EXIT_SUCCESS`;
     - runs `/bin/<name given by user>`(and in `/usr/bin/<name given by user>`, if the given command is not found in `/bin/`), through the [fork(2)](http://man7.org/linux/man-pages/man2/fork.2.html) and [exec(3)](http://man7.org/linux/man-pages/man3/exec.3.html) system call. It also give an appropriate error message(using[perror(3)](http://man7.org/linux/man-pages/man3/perror.3.html)) if exec() fails.
     - use [wait(2)](http://man7.org/linux/man-pages/man2/waitpid.2.html) to wait for child process to end;
   - The [lab1-23](SET_LAB/lab1_23.c): runs `ls -l >` *filename*(which is passed in `argv[1]`), using the [close(2)](http://man7.org/linux/man-pages/man2/close.2.html) and [dup(2)](http://man7.org/linux/man-pages/man2/dup.2.html) system call.
