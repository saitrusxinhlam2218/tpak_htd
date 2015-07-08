static char sz__FILE__[]="%Z%%M%	%I%(ver) Date Release %G%" ;
/****************************************************************************
*
*                           S C C S   I N F O
*
* %Z%  %M%; Rel: %R%.%L%.%B%.%S%; Get date: %H% at %T%
* %Z%  Last delta: %G% at %U%
* %Z%  SCCS File: %P%
*
* Copyright (c) 1995 - TaxiCom ApS
*
*****************************************************************************/

#include <stdio.h>
#include <time.h>


#define ONEDAY (60*60*24)

#define ENHSWITCHFILE "/usr/taxi/run/enh.cfg"
#define NO_SWITCH 32
struct SwitchFile
{
  int On;
  int Time;
  char Title[60];
} EnhBit[NO_SWITCH];



main(argc,argv)
int argc;
char *argv[];
{
  int i,j;
  unsigned int Remove=-1,Set=-1,List=0,Create=0;
  char Key[100],*Par[5];
  extern char *optarg;

  Key[0]=0;
  while((i=getopt(argc,argv,"k:r:s:lc"))!=EOF)
  {
    switch(i)
    {
			case 'k':
				if(!strcmp(optarg,"TaxiPak")) strcpy(Key,optarg); else Key[0]=0;
			break;
      case 'r': Remove=atoi(optarg)-1; break;
      case 's': Set=atoi(optarg)-1; break;
      case 'l': List=1; break;
      case 'c': Create=1; break;
      default:
        printf("syntax: %s [-k#r#s#lc]\n",argv[0]);
        printf("  options:\n");
				printf("       -k<key>                -> cryptkey\n");
				printf("       -r<n>                  -> remove enhancment 1-32\n");
				printf("       -s<n> <title> [<days>] -> set enhancement 1-32 \n");
				printf("       -l                     -> list enhancements\n");
				printf("       -c                     -> create new enhancement switch file\n");
      exit(0);
    }
  }

  if(Key[0] && List)
  {
    ReadFile(); Eor(Key);
    for(i=0; i<NO_SWITCH; i++)
    {
      if(EnhBit[i].On)
      {
				printf("Switch# %d, Title \"%s\"",i+1,EnhBit[i].Title);
				j=EnhBit[i].Time; if(j || 1) printf(", Date %s",ctime(&j));
        printf("\n");
      }
    }
  }
  else if(Key[0] && Create)
  {
    for(i=0; i<NO_SWITCH; i++)
    {
      EnhBit[i].On=0;
      EnhBit[i].Title[0]=0;
      EnhBit[i].Time=0;
    }
    Eor(Key); WriteFile();
  }
  else if(Key[0] && Set>=0 && Set<32)
  {
    ReadFile(); Eor(Key);
    for(i=1,j=0; i<argc && j<2; i++) if(argv[i][0]!='-') Par[j++]=argv[i];
    while(j<4) Par[j++]="";
		EnhBit[Set].On=1;
		strncpy(EnhBit[Set].Title,Par[0],59); EnhBit[Set].Title[59]=0;
		if(Par[1][0]) EnhBit[Set].Time=time(0)+atoi(Par[1])*ONEDAY;
		else EnhBit[Set].Time=0;
    Eor(Key); WriteFile();
  }
  else if(Key[0] && Remove>=0 && Remove<32)
  {
    ReadFile(); Eor(Key);
    EnhBit[Remove].On=0;
    EnhBit[Remove].Title[0]=0;
    EnhBit[Remove].Time=0;
    Eor(Key); WriteFile();
  }
  else
  {
    printf("syntax: %s [-lr#k#s#d#]\n",argv[0]);
    printf("  options:\n");
		printf("       -k<key>                -> cryptkey\n");
		printf("       -r<n>                  -> remove enhancment 1-32\n");
		printf("       -s<n> <title> [<days>] -> set enhancement 1-32 \n");
		printf("       -l                     -> list enhancements\n");
		printf("       -c                     -> create new enhancement switch file\n");
  }
}


ReadFile()
{
  FILE *hnd;

  if(hnd=fopen(ENHSWITCHFILE,"r"))
  {
    fread(EnhBit,1,sizeof(EnhBit),hnd);
    fclose(hnd);
  }
}

WriteFile()
{
  FILE *hnd;

  if(hnd=fopen(ENHSWITCHFILE,"w"))
  {
    fwrite(EnhBit,1,sizeof(EnhBit),hnd);
    fclose(hnd);
  }
}

Eor(key)
char *key;
{
  int i,keylen;
  char *ptr;

  ptr=(char *)EnhBit;
  keylen=strlen(key);
  for(i=0; i<sizeof(EnhBit); i++) ptr[i]^=key[i%keylen];
}
