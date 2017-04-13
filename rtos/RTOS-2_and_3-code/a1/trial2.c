#include<stdio.h>
#include<string.h>

void main()
{char str[200];
 while(fgets(str, sizeof str, stdin) != NULL)
{printf("%s\n",str);}
}
