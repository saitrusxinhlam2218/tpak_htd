static char sz__FILE__[]="@(#)db.c	10.1.2.2(ver) Date Release 5/11/95" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  db.c; Rel: 10.1.2.2; Get date: 5/11/95 at 09:59:02
 * @(#)  Last delta: 5/11/95 at 09:58:56
 * @(#)  SCCS File: /taxi/sccs/s.db.c
 *                                                                           *
 *****************************************************************************/

/****************************************************************************
 * History:                                                                 *
 *   ?/?/87  Eric Lui     Initial implementation                            *
 *   2/8/88  Jack Chou    Add redundancy capability for ADD_REC, UPDATE_REC,*
 *                        and DELETE.                                       *
*****************************************************************************/

#include <stdio.h>
#include <isam.h>
#include <sys/types.h>

#include "mad_error.h"
#include "mads_types.h"
#include "mads_isam.h"

#define DECLARING
#include "db_prof.h"

#define MAX_BUF        BUFSIZ 
#define  MASK          0xf

char    buf[BUFSIZ];
char    req_buf[BUFSIZ];
char    tmp_req_buf[BUFSIZ];
char    el_req_buf[BUFSIZ];

db(file_id,action_code,key_str,mode,rec_nbr)
int file_id;
int action_code;
struct keydesc *key_str;
int mode;
int rec_nbr;
{
	register int cc;

	switch(action_code)
	{
	case READ_KEYS:
		cc =db_read_key(file_id, req_buf, key_str, mode );
		break;
		
	case READ_NUM:
		cc =db_read_by_record_number(file_id, rec_nbr, req_buf );
		break;
		
	case UPDATE_REC:
		cc=db_update( file_id, req_buf );
		break;

	case START:
		cc=start(file_id,key_str,WHOLEKEY,buf,mode);
		break;
		
	case ADD_REC:
		cc= db_write( file_id, req_buf );
		break;
	case LOCK:
		cc = db_lock( file_id );
		break;
		
	case UNLOCK:
		cc = db_unlock( file_id );
		break;
		
	case RELEASE:
		cc=db_release( file_id );
		break;
		
	case OPEN:
		cc= db_open( file_id, mode );
		break;
		
	case CLOSE:
		cc= db_close( file_id );
		break;

	case DELETE:
		cc=db_delete( file_id, req_buf );
		break;
		
	default:
		break;
	}

	if ( cc == SUCCESS )
	  return( 0 );
	
	return ( -1 );
}

start(fd,key_str,len,buff,mode)
int  fd;
struct  keydesc  *key_str;
int  len;
char  *buff;
int   mode;
{
	register int cc;

	cc = isstart(fd,key_str,WHOLEKEY,buff,(mode & MASK));
	return(cc);
}




