# Another crappy repository!
This repository contains all my project for the *Trasmission and Processing System* class I'm currently attending!

## Project's list:
### 1. fork and exec:
   - [lab1-22](lab1_22.c):
     - waits for a command name to be entered(to get user's input I used the [fgets(3)](http://man7.org/linux/man-pages/man3/gets.3.html) system call );
     - if `exit` or `^D` are entered, exits using [exit(3)](http://man7.org/linux/man-pages/man3/exit.3.html) with *exit status* `EXIT_SUCCESS`;
     - runs `/bin/<name given by user>`(or `/usr/bin/<name given by user>`, if the given command is not found in `/bin/`), through the [fork(2)](http://man7.org/linux/man-pages/man2/fork.2.html) and [exec(3)](http://man7.org/linux/man-pages/man3/exec.3.html) system call. It also give an appropriate error message(using[perror(3)](http://man7.org/linux/man-pages/man3/perror.3.html)) if exec() fails.
     - use [wait(2)](http://man7.org/linux/man-pages/man2/waitpid.2.html) to wait for child process to end;
   - [lab1-23](lab1_23.c): performs `ls -l >` *filename*(which is passed in `argv[1]`), using the [close(2)](http://man7.org/linux/man-pages/man2/close.2.html) and [dup(2)](http://man7.org/linux/man-pages/man2/dup.2.html) system call.
   - [lab1_24](lab1_24.c)**(wip)**: performs `ps aux | grep bash` using the [pipe(2)](http://man7.org/linux/man-pages/man2/pipe.2.html) system call. 

### 2. ping-pong:
This project is a bit more complicated, it consist of a tcp client, a udp client and a server [pong]();
- [tcp_ping](): (usage `server_address server port size ripetition`)
   implement a tcp client that 
   - uses [getaddrinfo(3)](http://man7.org/linux/man-pages/man3/getaddrinfo.3.html) to obtain server's address in binary form;
   - uses [socket(2)](http://man7.org/linux/man-pages/man2/socket.2.html) to create a socket; 
   - uses [connect(2)](http://man7.org/linux/man-pages/man2/connect.2.html) to link the socket to the server's address and port;
   - send and receive back data from server with the [send(2)](http://man7.org/linux/man-pages/man2/send.2.html) and [recv(2)](http://man7.org/linux/man-pages/man2/recv.2.html) system call;
