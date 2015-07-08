/***************************************************************/
/* This program will download a new password to a terminal.    */
/*                                                             */
/* This program was made by : Jesper Blom                      */
/*                            Zenitel  - Copenhagen            */
/*                            January, 2006                    */
/***************************************************************/

#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

int send_to_E2P(mid, mmp_string)
unsigned short int mid;
unsigned char *mmp_string;
{
	int fd,i,len;
	unsigned char buf[200];

	if((!mid) || (mid >= 0xfff)) return(-1);

	if((fd=open("/usr/taxi/run/to_async_pipe",O_RDWR,0)) > 0)
	{
		i=1;
		buf[i++]=0;
		buf[i++]=0;
		buf[i++]=0;
		sprintf(&buf[i],"%05X", mid);
		i+=5;
		buf[i++]='0';
		buf[i++]='0';
		buf[i++]='8';
		buf[i++]=' ';
		if(mid >= 0xf00) buf[i++]='M'; else buf[i++]=' ';
		buf[i++]=' ';
		buf[i++]='1';
		buf[i++]='3';
		buf[i++]='2';
		buf[i++]='0';
		buf[i++]='0';
		buf[i++]='0';
		len=strlen(mmp_string);
		strcpy(&buf[i], mmp_string);
		i+=len;
		buf[i++]='%';
		buf[i++]='E';
		buf[i++]=0x7c;
		buf[i++]=0;
		buf[0]=i-4;
		write(fd,buf,i);
		close(fd);
		return(0);
	}
	return(-1);
}

main(argc, argv)
int argc;
char *argv[];
{
	unsigned char buf[200];
	int i,mid;

	if(argc != 3)
	{
	printf("\n <mid nr> <Parameter>\n\n");
	return;
	}
	mid=atoi(argv[1]);
	if((mid <1) || (mid > 0xffe))
	{
	printf("\n MID nr. forkert indtastet!\n\n");
	return;
	}
	if(strlen(argv[2]) != 8)
	{
	printf("\n Parameter skal være 8 ciffer!\n\n");
	return;
	}
	/*for(i=0; i<8; i++)
	 {
	  if((argv[2][i] > '9') || ((argv[2][i] < '0') && (argv[2][i] != ' ')))
	  {
	  printf("\n Der kan kun indtastes tal i Parameer!\n\n");
	  return;
	  }
    }*/
	sprintf(buf,"%s",argv[2]);
	if(send_to_E2P(mid, buf)) return(-2000);
	return(0);
}
