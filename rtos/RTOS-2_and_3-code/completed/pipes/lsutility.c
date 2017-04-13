#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    int pfds[2];//pipe returns two file descriptors that refer to the end of the pipe:stdin and stdout

    pipe(pfds);//creates a unidirectional communication channel called pipe

    if (!fork()) //fork off a child process,code for the child process
    {
        close(1);       /* close normal stdout */
        dup(pfds[1]);   /* make stdout same as pfds[1] ;dup creates a copy of the file descriptor which in this case is stdout*/
        close(pfds[0]); /* we don't need this;close the other end because it is duplicated anyway */
        execlp("ls", "ls", NULL);//execute ls
    } 
    else //code for the parent process
    {
        close(0);       /* close normal stdin */
        dup(pfds[0]);   /* make stdin same as pfds[0] */
        close(pfds[1]); /* we don't need this */
        execlp("wc", "wc","-l" ,NULL);//print newline,word and byte counts for each file
    }

    return 0;
}
