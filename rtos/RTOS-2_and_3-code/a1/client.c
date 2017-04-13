#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>//ftok
#include <sys/msg.h>

pid_t getpid(void);

//Q2)client inputs two operands and an operator and sends it to the server
struct my_msgbuf {
    long mtype;//type
    char mtext[200];//message
};

int main(void)
{
    struct my_msgbuf buf;
    int msqid,i,l;
    key_t key;
     char pid[10];

	
	
	snprintf(pid, 6,"%d",(int)getpid());
	//printf("%s\n",pid); 
	l=strlen(pid);

	//printf("%d",l);
    if ((key = ftok("client.c", 'B')) == -1) {//ftok converts a pathname(and also a project id-B) to an IPC key and returns an object of type key_t;
        perror("ftok");
        exit(1);
    }

    if ((msqid = msgget(key, 0644 | IPC_CREAT)) == -1) {//msgget takes in the key and returns a message queue identifier,create if queue doesn't already exist,644-read write owner,read-user group
        perror("msgget");
        exit(1);
    }
    
    printf("Enter an arithmetic expression in the form operand1 operator operand2, ^D to quit:\n");

    buf.mtype = 1; /* we don't really care in this case */

    while(fgets(buf.mtext, sizeof buf.mtext, stdin) != NULL) {
        int len = strlen(buf.mtext);

        /* ditch newline at end, if it exists */
        //if (buf.mtext[len-1] == '\n');
	for(i=0;i<l;i++)
		buf.mtext[len-1+i]=pid[i];
	 buf.mtext[len-1+i] = '\0';
	//printf("%s %lu",buf.mtext,strlen(buf.mtext));
        if (msgsnd(msqid, &buf, strlen(buf.mtext), 0) == -1) /* +1 for '\0' *///send a msg
            perror("msgsnd");
	
    }

    if (msgctl(msqid, IPC_RMID, NULL) == -1) //remove the message queue awakening all reader and writer programs
    {
        perror("msgctl");
        exit(1);
    }

    return 0;
}
