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

#define PRINT_TEXT_STOCK     0x21
#define PRINT_TEXT           0x20

int send_to_E2P(mid, link, mmp_string, stockholm)
unsigned short int mid;
char link;
unsigned char *mmp_string;
int stockholm;
{
	int fd,i,len;
	unsigned char buf[200];

	if((!mid) || (mid >= 0xfff)) return(-1);

	if ( stockholm )
	  sprintf( mmp_string, "%02X%s", PRINT_TEXT_STOCK, mmp_string );
	else
	  sprintf( mmp_string, "%02X%s", PRINT_TEXT, mmp_string );
	if ( link == '8' )
	  fd = open("/usr/taxi/run/to_udp_pipe",O_RDWR,0);
	else
	  fd = open("/usr/taxi/run/to_async_pipe",O_RDWR,0);
	
	{
		i=1;
		buf[i++]=0;
		buf[i++]=0;
		buf[i++]=0;
		sprintf(&buf[i],"%05X", mid);
		i+=5;
		buf[i++]='0';
		buf[i++]='0';
		buf[i++]=link;
		buf[i++]=' ';
		if(mid >= 0xf00) buf[i++]='M'; else buf[i++]=' ';
		buf[i++]=' ';
		buf[i++]='2';
		buf[i++]='0';
		buf[i++]='0';
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
	char pkt_id[3];
	char link;
	int stockholm = 0;

	if(argc != 5)
	{
	printf("\n <mid nr> <link> <print_text> <0|1>\n\n");
	return;
	}
	mid=atoi(argv[1]);

	if((mid <1) || (mid > 0xffe))
	{
	printf("\n MID nr. forkert indtastet!\n\n");
	return;
	}

	link = argv[2][0];

	if ( argv[4][0] == '1' )
	  stockholm = 1;

	sprintf(buf,"%s",argv[3]);
	if(send_to_E2P(mid, link, buf, stockholm)) return(-2000);
	return(0);
}
