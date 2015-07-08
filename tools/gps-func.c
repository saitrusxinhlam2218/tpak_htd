#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

int send_to_gps(mid, command)
unsigned short int mid;
unsigned char *command;
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
		buf[i++]=' ';
		buf[i++]=' ';
		if(mid >= 0xf00) buf[i++]='M'; else buf[i++]=' ';
		buf[i++]=' ';
		buf[i++]='2';
		buf[i++]='0';
		buf[i++]='0';
		buf[i++]='1';
		buf[i++]='0';
		buf[i++]='0';
		len=strlen(command);
		strcpy(&buf[i], command);
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

int build_command(in_buf, len, out_buf)
unsigned char *in_buf;
unsigned int len;
unsigned char *out_buf;
{
  int i,ii;

  for(i=2; i<(len-3); i++) ii ^= in_buf[i];
  in_buf[len-3]=ii & 0xff;

  while(len % 3 != 0)
  {
    in_buf[len++]=0;
  }

  for(i=0, ii=0; len; len-=3, ii+=3)
  {
    out_buf[i]=((in_buf[ii] & 0xfc) >> 2) + 0x20;
    if(out_buf[i++] == '%') out_buf[i++]='%';
    out_buf[i]=(((in_buf[ii] & 0x03) << 4)
               + ((in_buf[ii+1] & 0xf0) >> 4)) + 0x20;
    if(out_buf[i++] == '%') out_buf[i++]='%';
    out_buf[i]=(((in_buf[ii+1] & 0x0f) << 2)
               + ((in_buf[ii+2] & 0xc0) >> 6)) + 0x20;
    if(out_buf[i++] == '%') out_buf[i++]='%';
    out_buf[i]=(in_buf[ii+2] & 0x3f) + 0x20;
    if(out_buf[i++] == '%') out_buf[i++]='%';
  }
  out_buf[i++]=0;
  return(i);
}


int get_command(mid)
unsigned short int mid;
{
	int i,ii,len;
	char *p,buf1[200],buf2[200];

	printf("\n1) Hent GPS status");
	printf("\n2) Sæt GPS status");
	printf("\n3) Positions opdaterings tid");
	printf("\n4) GMT korrektion");
	printf("\n5) Genstart GPS");
	printf("\n6) Datum");
	printf("\n7) Longitude");
	printf("\n8) Latitude");
	printf("\n9) Ionosphere");
	printf("\nIndtast funktions nummer : ");
	gets(buf1);
	i=atoi(buf1);
	switch(i)
	{
		case 1 :	sprintf(buf1,"@@Cg%c %c%c",2,13,10);
				len=8;
				build_command(buf1, len, buf2);
				if(send_to_gps(mid, buf2))
				{
					printf("\nDer er fejl i det indtastede !!");
					return(-1);
				}
				break;

		case 2 :	printf("\nIndtast ny status (1=Idle, 2=Fix) : ");
				gets(buf1);
				i=atoi(buf1);
				sprintf(buf1,"@@Cg%c %c%c",i-1,13,10);
				len=8;
				build_command(buf1, len, buf2);
				if(send_to_gps(mid, buf2))
				{
					printf("\nDer er fejl i det indtastede !!");
					return(-1);
				}
				break;

		case 3 :	printf("\nIndtast opdaterings hastigheden : ");
				gets(buf1);
				i=atoi(buf1);
				sprintf(buf1,"@@Ea%c %c%c",i,13,10);
				len=8;
				build_command(buf1, len, buf2);
				if(send_to_gps(mid, buf2))
				{
					printf("\nDer er fejl i det indtastede !!");
					return(-1);
				}
				break;

		case 4 :	printf("\nIndtast tids korrektionen : ");
				gets(buf1);
				i=atoi(buf1);
				if(i<0)
				{
					i=abs(i);
					sprintf(buf1,"@@Ab%c%c%c %c%c",255,i,0,13,10);
				}
				else
				{
					sprintf(buf1,"@@Ab%c%c%c %c%c",0,i,0,13,10);
				}
				len=10;
				build_command(buf1, len, buf2);
				if(send_to_gps(mid, buf2))
				{
					printf("\nDer er fejl i det indtastede !!");
					return(-1);
				}
				break;

		case 5 :	sprintf(buf1,"@@Cf %c%c",13,10);
				len=7;
				build_command(buf1, len, buf2);
				if(send_to_gps(mid, buf2))
				{
					printf("\nDer er fejl i det indtastede !!");
					return(-1);
				}
				sleep(10);
				sprintf(buf1,"@@Cf %c%c",13,10);
				len=7;
				build_command(buf1, len, buf2);
				if(send_to_gps(mid, buf2))
				{
					printf("\nDer er fejl i det indtastede !!");
					return(-2);
				}
				sleep(10);
				sprintf(buf1,"@@Ao%c %c%c",12,13,10);
				len=8;
				build_command(buf1, len, buf2);
				if(send_to_gps(mid, buf2))
				{
					printf("\nDer er fejl i det indtastede !!");
					return(-3);
				}
				sleep(5);
				sprintf(buf1,"@@Cg%c %c%c",1,13,10);
				len=8;
				build_command(buf1, len, buf2);
				if(send_to_gps(mid, buf2))
				{
					printf("\nDer er fejl i det indtastede !!");
					return(-4);
				}
				sleep(5);
				sprintf(buf1,"@@Ea%c %c%c",1,13,10);
				len=8;
				build_command(buf1, len, buf2);
				if(send_to_gps(mid, buf2))
				{
					printf("\nDer er fejl i det indtastede !!");
					return(-5);
				}
				printf("\nHusk : GMT korrektion");
				break;

		case 6 :	sprintf(buf1,"@@Ao%c %c%c",12,13,10);
				len=8;
				build_command(buf1, len, buf2);
				if(send_to_gps(mid, buf2))
				{
					printf("\nDer er fejl i det indtastede !!");
					return(-1);
				}
				break;

		case 7 :	sprintf(buf1,"@@Ae     %c%c",13,10);
				p=&buf1[7];
				i=((((12*60)+0)*60)+0)*1000;
				for(ii=0; ii<4; ii++)
				{
					*p--=(unsigned char)(i % 256);
					i=i/256;
				}
				len=11;
				build_command(buf1, len, buf2);
				if(send_to_gps(mid, buf2))
				{
					printf("\nDer er fejl i det indtastede !!");
					return(-1);
				}
				break;

		case 8 :	sprintf(buf1,"@@Ad     %c%c",13,10);
				p=&buf1[7];
				i=((((55*60)+0)*60)+0)*1000;
				for(ii=0; ii<4; ii++)
				{
					*p--=(unsigned char)(i % 256);
					i=i/256;
				}
				len=11;
				build_command(buf1, len, buf2);
				if(send_to_gps(mid, buf2))
				{
					printf("\nDer er fejl i det indtastede !!");
					return(-1);
				}
				break;

		case 9 :	printf("\nIndtast status (Enable/Disable) : ");
				gets(buf1);
				buf1[0]=toupper(buf1[0]);
				if((buf1[0] != 'E') && (buf1[0] != 'D'))
				{
					printf("\nDer er fejl i det indtastede !!");
					return(-1);
				}
				sprintf(buf1,"@@Aq%c %c%c",buf1[0]=='E'?1:0,13,10);
				len=8;
				build_command(buf1, len, buf2);
				if(send_to_gps(mid, buf2))
				{
					printf("\nDer er fejl i det indtastede !!");
					return(-1);
				}
				break;

		default :	return(-100);
	}
	return(0);
}

main(argc, argv)
int argc;
char *argv[];
{
	unsigned char buf[20];
	unsigned short int mid;
	int i;

	printf("\nGPS-FUNC\n\nIndtast MID nummer : ");
	gets(buf);
	mid=atoi(buf);
	if(i=get_command(mid))
	{
		printf("\nFejl %d i kommando delen !\n",i);
		return;
        }
	printf("\n");
}
