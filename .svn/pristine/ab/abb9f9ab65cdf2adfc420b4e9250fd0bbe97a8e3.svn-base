#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include "credit.h"

typedef unsigned char byte;
#define UNKNOWN_ACCT_TYPE '?'
main( argc, argv )
     int argc;
     char **argv;
{
  char dest_buf[1024];
  char scratch_buf[1024];
  char source_buf[1024];
  char data_str[1024];
  int  len = 0, i, j;
  int done = 0;
  FILE  *fp;
  char    filename[64];
  char    *pDelimit;
  char    sDate[16];
  char    sTime[16];
  char    cIO;
  int     iSeqNo, iLink;
  char    sMID[16];
  char    sPktID[16];
  char    sPFields[16];
  char    sMsg[512];
  char    sDescript[132];
  char    *pResult = NULL;
  char    *str_ptr = NULL;
  struct verify_acct_info acct_info;
  
  bzero( dest_buf, 1024 );
  bzero( scratch_buf, 1024 );
  bzero( source_buf, 1024 );

  sprintf( filename, "/usr/taxi/run/traces/trace_log.%s", argv[1] );
  fp = fopen( filename, "r" );
  if ( fp == NULL )
    exit(-1);

  while ( !done )
    {
      bzero( scratch_buf, 1024 );      
      pResult = fgets( sMsg, 1024, fp );
      if ( pResult == NULL )
	exit(0);

      pDelimit = NULL;
      pDelimit = strstr( sMsg, argv[2] );
      if ( pDelimit != NULL )
	{
	  pDelimit = NULL;
	  pDelimit = strstr( sMsg, "ITM-CARD_VALIDITY" );
	  if ( pDelimit != NULL )
	    {
	      pDelimit = (pDelimit + 20);
	      strcpy( source_buf, pDelimit );
	      Unescape( source_buf );
	      source_buf[strlen(source_buf)-3] = '\0';
	      len = expand( dest_buf, source_buf, strlen( source_buf ) );
	      pDelimit = strstr( &dest_buf[1], ":" );
	      if ( pDelimit != NULL )
		{
		  *pDelimit = '\0';
		  sMsg[13] = '\0';
		  printf( "RQ_CARDVAL\t%-13s %x %s\n", sMsg, dest_buf[0], &dest_buf[1] );
		}
	    }
	  else if ( ( pDelimit = strstr( sMsg, "ITM-RQ_CARD_VAL 64" ) ) != NULL )
	    {
	      pDelimit = (pDelimit + 14);
	      strcpy( source_buf, pDelimit );
	      Unescape( source_buf );
	      i = 0;
	      while (source_buf[i] != '\0')
		i++;
	      len = expand( dest_buf, source_buf, i-4 );
	      j = 0;
	      while (i && j< 200)
		{
		  sprintf(&data_str[2*j],"%c%c", (dest_buf[j] & 0xF0)/16+'0',(dest_buf[j] & 0x0F)+'0');
		  i--;
		  j++;
		}
	      data_str[2*j]=0;       /* terminate string */

	      str_ptr = strchr(&data_str[2], ':');
	      str_ptr++;
	      acct_info.acct_type = UNKNOWN_ACCT_TYPE;
	      acct_info.amount[0] = '\0';
	      acct_info.expire_date[0] = '\0';
	      acct_info.card_name[0] = '\0';
	      strcpy(acct_info.acct_nbr,str_ptr);
	      check_acct_type_file( &acct_info );
	      
	      sMsg[13] = '\0';
	      printf( "CARD_VALIDITY\t%-13s %x %s (%s)\n", sMsg, dest_buf[0], &data_str[2], acct_info.card_name );
	    }
	}
    }
}
  
  
int compress(byte *dest, byte *source, int len) {

    int i = 0, j;
    byte *pntr, rem;
    pntr = dest;
    j = 4 * (len / 3);
    if(rem = len % 3) j += rem + 1;


    while(j--) {

	switch(i++ & 0x3) {

	    case 0:
		*pntr++ = *source >> 2;
		break;

	    case 1:
		*pntr = (*source++ << 4) & 0x30;
		*pntr++ |= *source >> 4;
		break;

	    case 2:
		*pntr = (*source++ << 2) & 0x3C;
		*pntr++ |= *source >> 6;
		break;

	    case 3:
		*pntr++ = *source++ & 0x3F;
		break;
	}

    }

    i = len = (int)(pntr - dest);

    while(i--) *dest++ += ' ';

    return(len);

}

int expand(byte *dest, byte *source, int len) {

    int i = 0, j;
    byte *pntr, rem;
    j = 3 * (len / 4);
    if(rem = len % 4) j += (rem-1);
    pntr = source;
    while(len--) *pntr++ -= ' ';
    pntr = dest;

    while(j--) {

	switch(i++ % 3) {

	    case 0:
		*pntr = *source++ << 2;
		*pntr++ |= (*source >> 4) & 0x3;
		break;

	    case 1:
		*pntr = *source++ << 4;
		*pntr++ |= (*source >> 2) &  0xF;
		break;

	    case 2:
		*pntr = *source++ << 6;
		*pntr++ |= *source++ & 0x3F;
		break;

	}

    }

    return((int)(pntr - dest));
}

Unescape( char *message )
{
  int i, j, len;
  char scratch_buf[1024];
  char scratch2_buf[1024];  
  char *pTmp, *pEscape;
  int  done = 0;
  int  bModified = 0;

  strcpy( scratch_buf, message );
  while ( !done )
    {
      bzero( scratch2_buf, 1024 );
      pTmp = &scratch_buf[0];
      pEscape = strstr( scratch_buf, "%%" );
      if ( pEscape == NULL )
	done = 1;
      else
	{
	  bModified = 1;
	  *pEscape = '\0';
	  strcat( scratch2_buf, pTmp );
	  strcat( scratch2_buf, (pEscape + 1) );
	  strcpy( scratch_buf, scratch2_buf );
	}
    }
  if ( bModified )
    strcpy( message, scratch_buf );
    
}
	
 int
 check_acct_type_file(struct verify_acct_info *acct_info)
{
	int i,j;
	char  record[MAX_CREDIT_RECORD_LEN];
	FILE *acct_type_fp;

	acct_type_fp = fopen("/usr/taxi/etc/acct_type_file", "r");
	if (acct_type_fp == NULL)
	{
		return (0);
	}
	while (fgets(record, MAX_CREDIT_RECORD_LEN, acct_type_fp) != NULL)
	{
		j=0;
		while ((record[j]!=' ')&&(record[j]!=NULL)&&(record[j]!='*'))
		{
			j++;
		}
		record[j] = NULL;
		if(strncmp(record+1, acct_info->acct_nbr, strlen(record+1))==0)
		{
			if(record[0]=='-')
			{
				fclose(acct_type_fp);
				get_card_name(acct_info->card_name, &record[j+1]);
				return(-1);    /* no do not go check the bad account file*/
			}
			else
			{
				fclose(acct_type_fp);
				get_card_name(acct_info->card_name, &record[j+1]);
				return(0);		/* yes go check bad account file*/
			}
		}
	}
	fclose(acct_type_fp);
	return(-1);		/*assume yes go check bad account file*/
}				/* end check_acct_type_file */

get_card_name(char *card_name, char *record)
{

  char *str_ptr,*dum_ptr;
  int j;

  /***********************************
   * Check if card name present      *
   ***********************************/
  if ((str_ptr = (char *)strchr(record,'#')) == NULL)
  {
    /* set card_name to Unknown */
    strcpy(card_name,"UNKNOWN");

    return;
  }
  str_ptr += 2;

  if ((dum_ptr = (char *)strchr(str_ptr,'#')) == NULL)
  {
    /* No comment found */
    j = strlen(str_ptr);
    while(((*(str_ptr+j) == ' ') || (*(str_ptr+j) == '\t')) && (j != 0))
      j--;
    if (j != 0)
      *(str_ptr + j + 1) = NULL;
  }
  else
  {
    /* Comment found */
    j = 1;
    while (((*(dum_ptr-j) == ' ') || (*(dum_ptr-j) == '\t')) && (str_ptr <= dum_ptr-j))
      j++;
    *(dum_ptr - j + 1) = NULL;

  }
  strcpy(card_name,str_ptr);
/*  j = 0;
  while (card_name[j] != NULL)
  {
    card_name[j] = Toupper(card_name[j]);
    j++;
  } */
} /* end get_card_name */
