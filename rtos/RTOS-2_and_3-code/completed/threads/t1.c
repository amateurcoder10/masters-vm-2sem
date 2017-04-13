#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h> 

char n[1024];
sem_t len;//semaphore that synchronises read and write--write has to wait for read to unlock

void * read1()
{
    while(1)
    {
          printf("\nThread-1:Enter a string\n");
          scanf("%s",n);
          printf("\n");
          sem_post(&len);//release semaphore;increments it--if greater than zero another process waiting for it unblocks
    }
}

void * write1()
{
        while(1)
        {
           sem_wait(&len);//lock before writing;decrements variable if greater than zero;else blocks
           printf("\nThread-2:The string entered is :");
           printf("==== %s\n",n);
        }

}

int main()
{
        int status;
       pthread_t tr, tw;//2 threads to read and write
       char *ptr ;
       const int x = 100;
	//printf("the value of sem in main is %d\n",*len);	
       ptr = (char*) malloc(100);
       printf("Address of pointer variable  is %p \n", &ptr);//pointer on the stack 
       printf("Address on heap is %p\n ", ptr);//it holds the address of dynmically allocated memory which is on the heap
       printf("Address on const  is %p\n ", &x);//write protected region of the stack
       printf("Address on global  is %p \n", &len);//global variable in th text segment
        pthread_create(&tr,NULL,read1,NULL);//create a read thread with runner 
        pthread_create(&tw,NULL,write1,NULL);//write thread

        printf("Pthread value = %lu\n", tr);//return value
        printf("Pthread value = %lu\n", tw);
        pthread_join(tr,NULL);//wait for termination
        pthread_join(tw,NULL);
}
