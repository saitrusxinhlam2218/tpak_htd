static char sz__FILE__[]="@(#)EnhSwitch.c	10.1.3.2(ver) Date Release 8/9/95" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  EnhSwitch.c; Rel: 10.1.3.2; Get date: 8/9/95 at 09:16:04		            *			
* @(#)  Last delta: 8/9/95 at 09:15:37						    *	
* @(#)  SCCS File: /taxi/sccs/s.EnhSwitch.c							    *
*                                                                           *
* Copyright (c) 1994 - TaxiCom ApS					    *
*                                                                           *
*****************************************************************************/

#include "enhance_com.h"
#include "switch_enh.h"
#include <stdio.h>

#define ENHSWITCHFILE "/usr/taxi/run/enh.cfg"
#define NO_SWITCH 32
struct SwitchFile
{
	int On;
	int Time;
	char Title[60];
} EnhBit[NO_SWITCH];


 /**************************************************************************
  * Name: int enh(long *glEnhSwitch)					   *
  * Type:                                                          *
  *                                                                        * 
  * Purpose: Sets the bits in the enhancement bit mask 'glEnhSwitch'.      * 
  *           		   		 				   *
  *                                                                        *
  * Returnvalue: On success : 1				                   *
  *              On failure : -1				           *
  **************************************************************************/
int enh(long *glEnhSwitch)
{
	int i;
	long enhancement=0;

	ReadFile(); Eor("TaxiPak");
	for(i=0; i<NO_SWITCH; i++)
	{
		if(EnhBit[i].On && (!EnhBit[i].Time || EnhBit[i].Time>time(0))) enhancement|=1<<i;
	}
	*glEnhSwitch = enhancement;
  return(1);
} /* end enh */


ReadFile()
{
	FILE *hnd;

	if(hnd=fopen(ENHSWITCHFILE,"r"))
	{
		fread(EnhBit,1,sizeof(EnhBit),hnd);
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
