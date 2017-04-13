#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include<string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct my_msgbuf {
    long mtype;
    char mtext[200];
};

int main(void)
{
    struct my_msgbuf buf;
    int msqid;
	
    int op1,op2;float res;
	char cid[6];
	char op;
	char* str="ClientID operand1 operator operand2 result\n";
    key_t key;
	FILE *fd;
	int l,i;
	fd=fopen("res.txt","a");
	//printf("opening file\n");
	if(fd==NULL)
		printf("file error");
	fprintf(fd,"%s",str);
	fclose(fd);
    if ((key = ftok("client.c", 'B')) == -1) {  /* same key as kirk.c */
        perror("ftok");
        exit(1);
    }

    if ((msqid = msgget(key, 0644)) == -1) { /* connect to the queue */
        perror("msgget");
        exit(1);
    }
    
    printf("Server: ready to compute.Send me an expression\n");

    for(;;) { /* Spock never quits! */
        if (msgrcv(msqid, &buf, sizeof buf.mtext, 0, 0) == -1) 
        {
            perror("msgrcv");
            exit(1);
        }
        //printf("server: \"%s\"\n", buf.mtext);
	    op1=(int)buf.mtext[0]-'0';
		op2=(int)buf.mtext[2]-'0';
		op=(int)buf.mtext[1];
		//printf("%lu is the size of buf\n",strlen(buf.mtext));
		for(i=3;i<=strlen(buf.mtext);i++)
			{
			cid[i-3]=(int)buf.mtext[i];
			
			}
			
		printf("Received:%d %c %d\n",op1,op,op2);
		switch(op)
		{case '+':res=op1+op2;
				break;
		case '-':res=op1-op2;
				break;
		case '*':res=op1*op2;
				break;
		case '/':if(op2==0)
				{printf("division by zero error\n");
				continue;}
				res=(float)op1/op2;
				break;
		default:printf("unkown operator\n");
				break;
		}
	fd=fopen("res.txt","a");
	fprintf(fd,"\n%s\t%d\t%c\t%d\t%f\n",cid,op1,op,op2,res);
	fclose(fd);
	printf("Result of computation is:%f \n",res);
    }
	
    return 0;
}
