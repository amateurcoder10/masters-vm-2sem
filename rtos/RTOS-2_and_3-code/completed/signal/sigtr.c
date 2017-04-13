#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
void my_handler_for_sigint(int signumber)
{
  char ans[2];
  if (signumber == SIGINT)
  {
    printf("received SIGINT\n");
    printf("Program received a CTRL-C\n");
    printf("Terminate Y/N : "); 
    scanf("%s", ans);
    if (strcmp(ans,"Y") == 0)
    {
       printf("Existing ....\n");
       exit(0); 
    }
    else
    {
       printf("Continung ..\n");
    }
  }
}

int main(void)
{
   /* Registering the Signal handler */
  if (signal(SIGINT, my_handler_for_sigint) == SIG_ERR)
      printf("\ncan't catch SIGINT\n");
printf("outside the loop\n");
  // A long long wait so that we can easily issue a signal to this process
  while(1) 
	{printf("in the loop\n");
    sleep(1);}
  return 0;
}

