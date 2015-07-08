static char sz__FILE__[]="@(#)error.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  error.c; Rel: 6.3.0.0; Get date: 2/7/92 at 15:55:18
 * @(#)  Last delta: 2/7/92 at 14:12:14
 * @(#)  SCCS File: /taxi/sccs/s.error.c
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <ctype.h>
#include <string.h>

#include "taxipak.h"
#include "taxi_db.h"
#include "Error_db.h"
#include "path.h"

#define DB_FILE_NAME_LEN	7

extern int iserrno;
extern int iserrio;
extern int errno;
//extern long isrecnum;


/*******************************************************************************
 * mad_error - Write the error to a cisam file.
 *******************************************************************************/
void
mad_error(fleet_id,ifilename,descr,srcfile,srcline)
char fleet_id;
char ifilename[];
char descr[];
char srcfile[];
int srcline;
{
    FILE *fp = NULL;
    static BOOLEAN error_db_file_is_open = FALSE;
    static BOOLEAN first_time = TRUE;
    int i, start, len;
    struct cisam_el	  err;
    char *last_slash;
    RET_STATUS rc;
    CHAR *ctime(), *stime;
    time_t  tmtime;

    if ( fp == NULL )
      {
	fp = fopen( MAD_ERROR_FILE, "a" );
	if ( fp == NULL )
	  {
	    fprintf( stderr, "Can't open %s\n", MAD_ERROR_FILE );
	    fp = stderr;
	  }
	else
	  /* Make sure a process owned by anyone can write to file */
	  chmod( MAD_ERROR_FILE, 0666 );
      }
    
    err.fleet = fleet_id;
    if ( ifilename == NULL || ifilename[0] == '\0' || isspace(ifilename[0]) )
      err.file[0] = '\0';
    else {
      
      /*  Filename will include absolute path. Look for
       *  last occurence of '/' to strip the path.
       */
      
      last_slash = strrchr( ifilename, '/' );
      
      /* If no '/' then copy from start of filename */
      if ( last_slash == NULL )
	strncpy( err.file, ifilename, DB_FILE_NAME_LEN );
      else
	strncpy( err.file, last_slash + 1, DB_FILE_NAME_LEN );
      
      /* Make sure file name ends */
      err.file[DB_FILE_NAME_LEN] = EOS;
    }
    
    tmtime = time((time_t *) 0);
    stime = ctime(&tmtime);
    
    strncpy( err.source_file, srcfile, sizeof(err.source_file) - 1 );
    err.source_file[ sizeof(err.source_file) - 1 ] = EOS;
    
    err.line_nbr = srcline;
    
    strncpy( err.error_msg, descr, sizeof(err.error_msg) - 1 );
    err.error_msg[ sizeof(err.error_msg) - 1 ] = EOS;    /* Make sure error message ends */
    
    /* Null out any trailing white space */
    len = strlen( err.error_msg );
    for ( i = len - 1; i >= 0; i-- )
      if ( isspace( err.error_msg[i] ) )
	err.error_msg[i] = EOS;
      else
	break;
    
    if ( err.fleet < 'A' || err.fleet > 'Z' )
      err.fleet = '#';
    
    fprintf( fp, "%.16s fl = %c db = %s : %s : src %s @ %d\n",
	    &stime[4], err.fleet, err.file,
	    descr, srcfile, srcline );
    fclose(fp);
    return;
}
