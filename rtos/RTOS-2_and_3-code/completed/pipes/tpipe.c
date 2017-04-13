#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

int main(void)
{
        int     fd[2], nbytes;//array of two file descriptors to describe the ends of the pipe
        pid_t   childpid;
        char    string[] = "Data: 9d234fffccc44 \n";
        char    readbuffer[80];

        pipe(fd);//create a pipe
       
        childpid = fork();//fork off a new process
        if(childpid == -1)
        {
                perror("fork");
                exit(1);
        }

        if(childpid == 0)//child process
        {
                /* Child process closes up input side of pipe */
                close(fd[0]);

                /* Send "string" through the output side of pipe */
                printf("                   Child Process: %s \n",string);
                write(fd[1], string, (strlen(string)+1));
                exit(0);
        }
        else
        {
                /* Parent process closes up output side of pipe */
                close(fd[1]);

                /* Read in a string from the pipe */
                nbytes = read(fd[0], readbuffer, sizeof(readbuffer));
                printf("Parent Process: Received string: %s", readbuffer);
        }
        
        return(0);
}
