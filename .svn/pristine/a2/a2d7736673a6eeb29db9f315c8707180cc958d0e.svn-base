/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: mem_mgmt.c,v $
*  @(#)  $Revision: 1.6 $
*  @(#)  $Date: 2001/03/12 16:41:14 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/common/mem_mgmt.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: mem_mgmt.c,v 1.6 2001/03/12 16:41:14 taxiadm Exp $";

/**************************************************************************
*
*   mem_mgmt.c
*
*   Ed Morin
*
***************************************************************************/
#include <stdio.h>
#include <malloc.h>
#include <errno.h>
#include <time.h>
#include "taxipak.h"
#include "taxi_error.h"
#include "Object.h"
#include "Packet_ID_private.h"
#include "mad_ipc.h"
#include "path.h"
#include "mem_mgmt.h"

#define PROTOTYPE 1

FILE *g_mm_fp_log;		/* Log file stream pointer. */
int   g_mm_i_pid;		/* Global process id of process we are
				 * running in. */
char  g_mm_ca_logfile[BUFSIZ];	/* Log file name. */

extern int errno;		/* Error value passed to error handling
				 * routines */


/*
 * mm_calloc() - Call calloc and log if required
 */
void *
mm_calloc(unsigned l_u_num, unsigned l_u_size, char *l_cp_file, int l_i_line)
{
   RET_STATUS ret_value;
   char *l_cp, *calloc();

   /* Call calloc */
   if ((l_cp = calloc(l_u_num, l_u_size)) == NULL)
   {
      ret_value = err_encode(TAXI_OBJ, errno);
   }
   /* Look for a command we may need to take care of. */
   mm_cmd();

   /* If the file is open, log our activity. */
   if (g_mm_fp_log != NULL)
   {
      (void) fprintf(g_mm_fp_log, "%d: %s (%d): calloc(%d, %d) returned %0x\n",
	    g_mm_i_pid, l_cp_file, l_i_line, l_u_num, l_u_size, (int) l_cp);
      fflush(g_mm_fp_log);
   }
   return (l_cp);
}

/*
 * mm_cmd() - handle any command directed at us
 */
void
mm_cmd()
{

}

/*
 * mm_free() - Call free and log if required
 */
void
mm_free(char *l_cp_ptr, char *l_cp_file, int l_i_line)
{
  char    DateBuff[16];
  time_t  nowtime;
  char    *stime, *ctime();
   /* Call free */
   free(l_cp_ptr);

#ifdef FOO
   /* Look for a command we may need to take care of. */
   mm_cmd();

   /* If the file is open, log our activity. */
   if (g_mm_fp_log != NULL)
   {
      nowtime = time(0);
      get_arg_date(nowtime, DateBuff);
      stime = ctime( &nowtime );
      stime[19] = '\0';
      (void) fprintf(g_mm_fp_log, "%s %s %d: %s (%d): free(%0x)\n",
		     DateBuff, &stime[11], getpid(), l_cp_file, l_i_line, (int) l_cp_ptr);
      fflush(g_mm_fp_log);
   }
#endif
}

void
mm_pktid_free(char *l_cp_ptr, char *l_cp_file, int l_i_line)
{
  char    DateBuff[16];
  time_t  nowtime;
  char    *stime, *ctime();
  PKT_ID_STRUCT *pPacket;

  pPacket = (PKT_ID_STRUCT *)l_cp_ptr;
  if (g_mm_fp_log != NULL)
    {
      nowtime = time(0);
      get_arg_date(nowtime, DateBuff);
      stime = ctime( &nowtime );
      stime[19] = '\0';
      (void) fprintf(g_mm_fp_log, "%s %s %d: %s (%d): free(%0x) veh_nbr %d pkt_value %02x\n",
		     DateBuff, &stime[11], getpid(), l_cp_file, l_i_line, (int) l_cp_ptr,
		     pPacket->veh_nbr, pPacket->pkt_nbr);
      fflush(g_mm_fp_log);
    }
  
  /* Call free */
  free(l_cp_ptr);
  
  /* Look for a command we may need to take care of. */
  mm_cmd();


}

/*
 * mm_malloc() - Call malloc and log if required
 */
void *
mm_malloc(unsigned l_u_size, char *l_cp_file, int l_i_line)
{
   RET_STATUS ret_value;
   char *l_cp;
   char    DateBuff[16];
   time_t  nowtime;
   char    *stime;

   /* Call malloc */
   if ((l_cp = (char *)malloc(l_u_size)) == NULL)
   {
      ret_value = err_encode(TAXI_OBJ, errno);
   }
#ifdef FOO
   /* Look for a command we may need to take care of. */
   mm_cmd();


   /* If the file is open, log our activity. */
   if (g_mm_fp_log != NULL)
   {
      nowtime = time(0);
      get_arg_date(nowtime, DateBuff);
      stime = ctime( &nowtime );
      stime[19] = '\0';      
      (void) fprintf(g_mm_fp_log, "%s %s %d: %s (%d): malloc(%d) returned %0x\n",
		     DateBuff, &stime[11], getpid(), l_cp_file, l_i_line, l_u_size, (int) l_cp);
      fflush(g_mm_fp_log);
   }
#endif
   return (l_cp);
}

void *
mm_pktid_malloc(unsigned l_u_size, char *l_cp_file, int l_i_line, short veh_nbr)
{
   RET_STATUS ret_value;
   char *l_cp;
   char    DateBuff[16];
   time_t  nowtime;
   char    *stime;

   /* Call malloc */
   if ((l_cp = (char *)malloc(l_u_size)) == NULL)
   {
      ret_value = err_encode(TAXI_OBJ, errno);
   }
   /* Look for a command we may need to take care of. */
   mm_cmd();

   /* If the file is open, log our activity. */
   if (g_mm_fp_log != NULL)
   {
      nowtime = time(0);
      get_arg_date(nowtime, DateBuff);
      stime = ctime( &nowtime );
      stime[19] = '\0';      
      (void) fprintf(g_mm_fp_log, "%s %s %d: %s (%d): malloc(%d) returned %0x veh_nbr %d\n",
		     DateBuff, &stime[11], getpid(), l_cp_file, l_i_line, l_u_size, (int) l_cp, (short)veh_nbr);
      fflush(g_mm_fp_log);
   }
   return (l_cp);
}

/*
 * mm_realloc() - Call realloc and log if required
 */
void *
mm_realloc(char *l_cp_ptr, unsigned l_u_size, char *l_cp_file, int l_i_line)
{
   RET_STATUS ret_value;
   char *l_cp;

   /* Call realloc */
   if ((l_cp = (char *)realloc(l_cp_ptr, l_u_size)) == NULL)
   {
      ret_value = err_encode(TAXI_OBJ, errno);
   }
   /* Look for a command we may need to take care of. */
   mm_cmd();

   /* If the file is open, log our activity. */
   if (g_mm_fp_log != NULL)
   {
      (void) fprintf(g_mm_fp_log, "%d: %s (%d): realloc(%0x, %d) returned %0x\n",
      g_mm_i_pid, l_cp_file, l_i_line, (int) l_cp_ptr, l_u_size, (int) l_cp);
      fflush(g_mm_fp_log);
   }
   /* Call realloc */
   return (l_cp);
}
