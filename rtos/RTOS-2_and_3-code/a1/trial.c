#include <stdio.h>
#include <stdlib.h>


int main(void)
{

	int a=5;
  
	FILE *fd;
	fd=fopen("res.txt","a");
	printf("opening file\n");
	if(fd==NULL)
		printf("file error");
	fprintf(fd,"%s","hello");
}

strcpy(op1 ,strtok(buf.mtext, " "));
	strcpy(op,strtok(NULL," "));
	strcpy(op2,strtok(NULL," "));
	strcpy(cid,strtok(NULL," "));
	
			
		printf("Received:%s %s %s %s\n",op1,op,op2,cid);

scanf("%[^\n]s",buf.mtext);
	
        int len = strlen(buf.mtext);
	buf.mtext[len] = ' ';
	//printf("%d",len);
        /* ditch newline at end, if it exists */
        //if (buf.mtext[len-1] == '\n');
	for(i=0;i<l;i++)
		buf.mtext[len+i+1]=pid[i];
	 buf.mtext[len+i+1] = '\0';
