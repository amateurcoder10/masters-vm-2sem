#include<stdio.h>
#include<math.h>
#include<stdlib.h>
long decimalToBinary(int n) {
    int remainder; 
 long binary = 0, i = 1;
  
    while(n != 0) {
        remainder = n%2;
        n = n/2;
        binary= binary + (remainder*i);
        i = i*10;
    }
    return binary;
}


void main()
{FILE *fin,*fout;
char c;
int d,i,temp;
int bin;


fin=fopen("sample.txt","r");//open text file
fout=fopen("out.txt","w");
printf("Encoding:The contents of the input file are as follows:\n\n");
if(fin == NULL) 
   {  printf("Error in opening file");
      return;
   }
   do //encode using repitition code
   {
      c = fgetc(fin);
      if( feof(fin) )
      {
         break ;
      }
      printf("%c with ascii value %d\n", c,(int)c);
      //d=decimalToBinary((int)c);
      //printf("%d is the binary value\n",d);
      for(i=0;i<8;i++)
      {temp=(int)c & (int)(pow(2,7-i));
	if(temp)
       fprintf(fout,"%d%d%d",1,1,1);
	else
       fprintf(fout,"%d%d%d",0,0,0);
      }
   }while(1);

   fclose(fin);
   fclose(fout);


//decoding
printf("\nDecoding\n\n");
fout=fopen("out.txt","r");
if(fout == NULL) 
   {  printf("Error in opening file");
      return;
   }
   do 
   {for(int i=0;i<8;i++)
	
      {c = fgetc(fout)-'0';//printf("c is %d\n",c);
      if( feof(fout) )
      {
         exit(0);
      }
      if(c){temp+=pow(2,7-i);
	}
      
      //printf("\t%d",temp);
      c=fgetc(fout);c=fgetc(fout);
      }if(temp!=107)
      printf("The character is %c\n",temp);
	else
	printf("Encountered line feed\n");
      temp=0;
   }while(1);

return;
}


