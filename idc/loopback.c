#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

int main(int argc, char *argv[])
{
char line[1024];
int chkin;
char input[1024];
char msg[1024];
char serport[24];

// argv[1] - serial port
// argv[2] - file or echo 

sprintf(serport, "%s", argv[1]);

int file= open(serport, O_RDWR | O_NOCTTY | O_NDELAY);

if (file == 0)
{
sprintf(msg, "open_port: Unable to open %s.\n", serport);
perror(msg);
}
else
fcntl(file, F_SETFL, FNDELAY); //fcntl(file, F_SETFL, 0);

while (1)
{

printf("enter input data:\n");
scanf("%s",&input[0]);

chkin=write(file,input,sizeof input);

if (chkin<0)
{
printf("cannot write to port\n");
}

//chkin=read(file,line,sizeof line);

while ((chkin=read(file,line,sizeof line))>=0)
{
if (chkin<0)
{
printf("cannot read from port\n");
}
else
{
printf("bytes: %d, line=%s\n",chkin, line);
}
}

/*CODE TO EXIT THE LOOP GOES HERE*/
if (input[0] == 'q') break;
}

close(file);
return 0;
}

