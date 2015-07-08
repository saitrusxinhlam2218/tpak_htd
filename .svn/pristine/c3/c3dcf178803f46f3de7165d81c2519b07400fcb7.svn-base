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

int send_to_E2P(mid, pkt_id, mmp_string)
unsigned short int mid;
char *pkt_id;
unsigned char *mmp_string;
{
	int fd,i,len;
	unsigned char buf[200];

	if((!mid) || (mid >= 0xfff)) return(-1);

	if((fd=open("/usr/taxi/run/to_udp_pipe",O_RDWR,0)) > 0)
	{
		i=1;
		buf[i++]=0;
		buf[i++]=0;
		buf[i++]=0;
		sprintf(&buf[i],"%05X", mid);
		i+=5;
		buf[i++]=*pkt_id;
		buf[i++]=*(pkt_id+1);
		buf[i++]='8';
		buf[i++]=' ';
		if(mid >= 0xf00) buf[i++]='M'; else buf[i++]=' ';
		buf[i++]=' ';
		buf[i++]='2';
		buf[i++]='2';
		buf[i++]='1';
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

	if(argc != 4)
	{
	printf("\n <mid nr> <pkt_id> <Parameter>\n\n");
	return;
	}
	mid=atoi(argv[1]);

	if((mid <1) || (mid > 0xffe))
	{
	printf("\n MID nr. forkert indtastet!\n\n");
	return;
	}

	strncpy( pkt_id, argv[2], 2);


	sprintf(buf,"%s",argv[3]);
	if(send_to_E2P(mid, pkt_id, buf)) return(-2000);
	return(0);
}
