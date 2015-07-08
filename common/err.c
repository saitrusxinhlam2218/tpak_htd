/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: err.c,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 2003/01/28 15:56:28 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/common/err.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: err.c,v 1.3 2003/01/28 15:56:28 jwelch Exp $";

#include <stdio.h>
#include <time.h>

#include "taxipak.h"
#include "taxi_error.h"
#include "Object.h"

#include "mad_error.h"

#define ERR_CAT_FILE "/h/bb/gilbert/g7/dev/errmsg.cat"

void ERR_log_stamp();

static FILE *err_cat_file = NULL;


RET_STATUS
tp_error( value, msg, do_log, do_abort, file, line )
RET_STATUS   value;
char        *msg;
BOOLEAN      do_log;
BOOLEAN      do_abort;
char        *file;
int          line;
{
    char    log_msg[ 256 ];
    char    tuple_msg[ 18 ];
    char    time_buff[ 24 ];
    time_t  timestamp;

/*
 *  Default logging is TRUE, unless a particular error implies not to...
 */
    if ( do_log == DEFAULT_LOG )
    {
	switch( value )
	{
	default:
	    do_log = TRUE;
	}
    }

/*
 *  Default abort is FALSE, unless a particular error implies to do so...
 */
    if ( do_abort == DEFAULT_ABORT )
    {
	switch( value )
	{
	default:
	    do_abort = FALSE;
	}
    }

    if ( do_log == TRUE )
    {
	ERR_log_stamp( time_buff );
	ERR_format_tuple( value, tuple_msg );
	sprintf( log_msg, " %s - %s",
		               tuple_msg, msg );
/*
 *  Here we need to supply the actual code to log to the
 *  database...
 */
	mad_error( 'X', "", log_msg, file, line );
    }
    if ( do_abort )
	abort();
    return( value );
}

/*
 *  Diagnostic support....
 */


void ERR_get_text( ret_stat, msg )
RET_STATUS  ret_stat;
char       *msg;
{
    unsigned int  err_val, err_cat, err_gp;

    if ( ret_stat == SUCCESS )
	*msg = EOS;

    if ( err_cat_file == NULL )
	err_cat_file = (FILE *) catopen( ERR_CAT_FILE );

    err_val = err_value( ret_stat );
    err_gp = err_group( ret_stat );

    if ( err_gp > 0 )
    {
	err_cat = 256 - err_gp;   /* a HACK for now... equates cat with gp */
    }
    else
    {
	if ( err_val < ERR_OBJ_BEGIN )
	    err_cat = TAXI_OBJ;
	else
	{
	    err_cat = err_object( ret_stat );
/* don't    err_val -= (ERR_OBJ_BEGIN - 1);  /* errs shifted by BEGIN value */
	}
    }
    strcpy( msg,
            (char *)catgets( err_cat_file, err_cat, err_val, "** UNKNOWN ERROR **" ) );
    return;
}

void ERR_format_tuple( ret_stat, tuple )
RET_STATUS  ret_stat;
char       *tuple;
{
    sprintf( tuple, "<%d,%d,%d>",
             err_value(ret_stat), err_group(ret_stat), err_object(ret_stat));
}

void ERR_log_stamp( buffer )
char *buffer;
{
    struct tm  *now;
    time_t     stamp;

    stamp = time( NULL );
    now = localtime( &stamp );
    sprintf( buffer, "%4d-%02d-%02d %02d:%02d:%02d",
	    1900 + now->tm_year, 1 + now->tm_mon, now->tm_mday,
	    now->tm_hour, now->tm_min, now->tm_sec );
}
