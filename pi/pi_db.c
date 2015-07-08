static char sz__FILE__[]="@(#)pi_db.c	10.1.2.1(ver) Date Release 12/2/94";
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  pi_db.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:45:46
 * @(#)  Last delta: 12/2/94 at 18:36:32
 * @(#)  SCCS File: /taxi/sccs/s.pi_db.c
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

#include "mads_types.h"
#include "mads_isam.h"

#define DECLARING
#include "db_prof.h"

#define MAX_BUF        BUFSIZ 
#define  MASK          0xf

extern struct file_desc isam_file[];

char    buf[BUFSIZ];
char    req_buf[BUFSIZ];
char    el_req_buf[BUFSIZ];

db(file_id,action_code,key_str,mode,rec_nbr)
int file_id;
int action_code;
struct keydesc *key_str;
int mode;
int rec_nbr;
{
	register int cc;

	DB_SHM_INC(file_id, action_code);

	switch(action_code)
	{
	case READ_KEYS:
		cc =read_keys(isam_file[file_id].fd,buf,mode,key_str,isam_file[file_id].ctokey,isam_file[file_id].cisam_to_c);
		break;
	case READ_NUM:
		cc =read_num(isam_file[file_id].fd,buf,mode,key_str,rec_nbr,isam_file[file_id].cisam_to_c);
		break;
	case UPDATE_UNIQ:
		cc=update_uniq(isam_file[file_id].fd,isam_file[file_id].c_to_cisam);
		break;
	case UPDATE_REC:
		cc=update_isam(isam_file[file_id].fd,isam_file[file_id].c_to_cisam);
		break;
	case WRITE:
		cc= is_write(isam_file[file_id].fd,buf);
		break;
	case RE_WRITE:
		cc=re_write(isam_file[file_id].fd,buf);
		break;
	case START:
		cc=start(isam_file[file_id].fd,key_str,WHOLEKEY,buf,mode);
		break;
	case ADD_REC:
		cc=add_isam(isam_file[file_id].fd,isam_file[file_id].c_to_cisam);
		break;
	case LOCK:
		cc=is_lock(isam_file[file_id].fd);
		break;
	case UNLOCK:
	case RELEASE:
		cc=release(isam_file[file_id].fd);
		break;
	case OPEN:
		cc=isam_file[file_id].fd = is_open(isam_file[file_id].file_name,mode);
		break;
	case CLOSE:
		cc=is_close(&isam_file[file_id].fd);
		break;

	case DELETE_UNIQ:
		cc=delete_uniq(isam_file[file_id].fd,isam_file[file_id].ctokey);
		break;

	case DELETE:
		cc=delete(isam_file[file_id].fd,isam_file[file_id].ctokey);
		break;
	case BUILD:
		(void) build_cisam(isam_file[file_id].file_name,isam_file[file_id].size,key_str,mode);
		break;
	case ADDINX:
		cc=add_index(&isam_file[file_id].fd,isam_file[file_id].file_name,key_str);
		break;
	case REDN_ADD:
		action_code = ADD_REC;		/* Add record on reduntant unit only */
		cc = 0;
		break;
	default:
		break;
	}

	memset(buf,'\0',sizeof buf);
		
	return(cc);
}



/* sets a lock on a C-ISAM file */

is_lock(fd)
int   fd;
{
	register int cc;

	cc = islock(fd);
	return(cc);
}



/* removes a lock on a C-ISAM file */

unlock(fd)
int  fd;
{
	register int cc;

	cc = isunlock(fd);
	return(cc);
}



/* removes lovks on records */

release(fd)
int  fd;
{
	register int cc;

	cc = isrelease(fd);
	return(cc);
}


/* opens a C-ISAM file */

is_open(filename,mode)
char *filename;
int mode;
{
	int fd;

	fd = isopen(filename,mode);
	return(fd);
}



/* close a C-ISAM file */

is_close(fd)
int *fd;
{
	register int cc;

	cc = isclose(*fd);

	/* Reset the file descriptor */
	*fd = -1;
	return(cc);
}


/* creates a C-ISAM file */

build_cisam(filename,reclen,key_str,mode)
char *filename;
int reclen;
struct keydesc  *key_str;
int   mode;
{
	register int cc;
	int fd;

	fd = isbuild(filename, reclen, key_str, mode);
	is_close(&fd);
}

/* adds an index to a file */

add_index(fd,filename,key_str)
int   *fd;
char *filename;
struct keydesc  *key_str;
{
	register int cc;

	is_open(filename, ISINOUT+ISEXCLLOCK);	/* open file read/write with exclusive lock */
	cc = isaddindex(*fd,key_str);
	if (cc != SUCCEED) {
		is_close(fd);
	}
	is_close(fd);
	return(cc);
}



read_keys(fd,buff,mode,key_str,key_fill,unfill)
int fd;
char *buff;
int  mode;
struct  keydesc  *key_str;
int  (*key_fill)();
int  (*unfill)();
{
	register int cc;
	key_fill();			/* Place the search value in the record in the position of the key */
	if (( (mode & ISEQUAL) == ISEQUAL) || ((mode & ISGREAT) == ISGREAT) || ((mode & ISGTEQ) == ISGTEQ)){
		cc = start(fd,key_str,WHOLEKEY,buff,mode);
		if( cc != SUCCEED)   
			return(cc); 
	}
	cc = file_read(fd,buff,mode);	/* Read records as indicated by the mode parameter */
	unfill(); 			/* Load program variables from C-ISAM data record */
	return(cc);
}


read_num(fd,buff,mode,key_str,rec_nbr,unfill)
int fd;
char *buff;
int  mode;
struct  keydesc  *key_str;
int  rec_nbr;
int  (*unfill)();
{
	int tmp_parts;
	register int cc;

	tmp_parts = key_str->k_nparts;
	key_str->k_nparts = 0;	/* Choose physical order */
	isrecnum = rec_nbr;		/* Set record number to first record to be processed */
	cc = start(fd,key_str,WHOLEKEY,buff,mode);
	if( cc != SUCCEED) {
		key_str->k_nparts = tmp_parts;
		return(cc); 
	}
	cc = file_read(fd,buff,mode);	/* Read records as indicated by the mode parameter */
	unfill(); 			/* Load program variables from C-ISAM data record */
	key_str->k_nparts = tmp_parts;
	return(cc);
}


	
/* isstart select the index and the starting point in the index */

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


/* isread to read records as indicated by the MODE parameter */

file_read(fd, buff, mode)
int  fd;
char *buff;
int  mode;
{
	register int cc;

	cc = isread(fd, buff, mode);

	/* If we reach beginning or end of file (iserrno == 110)
	   then either the first or last record in the file. This
	   is done because the pointer counts past the EOF */
	if (iserrno == 110)
	  {
	    if (mode == ISNEXT)
	      cc = isread(fd, buff, ISLAST);
	    else if (mode == ISPREV)
	      cc = isread(fd, buff, ISFIRST);
	    cc = -1;
	    iserrno = 110; 
	  }
	return(cc);
}



/* iswrite to write a new record to a C-ISAM file */

is_write(fd,buff)
int  fd;
char *buff;
{
	register int cc;

	cc = iswrite(fd,buff);
	return(cc);
}




/* Adding Records to C-ISAM file */
add_isam(fd,fill)
int   fd;
int (*fill)();
{
	register int cc;

	fill(ADD_REC); /* Store program variables in C-ISAM data record */
	cc = is_write(fd,buf);
	return(cc);
}


re_write(fd,buff)
int  fd;
char *buff;
{
	register int cc;

	cc = isrewrite(fd,buff); /* Updating all key values except PRIMARY KEY */
	return(cc);
}


rew_curr(fd,buff)
int  fd;
char *buff;
{
	register int cc;

	cc = isrewcurr(fd,buff); /* Updating all key values except PRIMARY KEY */
	return(cc);
}



/* updates a record in a C-ISAM file, use unique key only*/
update_uniq(fd,fill)
int    fd;
int    (*fill)();
{
	register int cc;

	fill(UPDATE_UNIQ);
	cc = re_write(fd,buf);
	return(cc);
}

update_isam(fd,fill)
int    fd;
int    (*fill)();
{
	register int cc;

	fill(UPDATE_REC);
	cc = rew_curr(fd,buf);
	return(cc);
}




is_delete_uniq(fd,buff)
int  fd;
char *buff;
{
	register int cc;

	cc = isdelete(fd,buff);
	return(cc);
}
is_delete(fd)
int  fd;
{
	register int cc;

	cc = isdelcurr(fd);
	return(cc);
}


/* deletes a C-ISAM record using unique key only */
delete_uniq(fd,key_fill)
int  fd;
int (*key_fill)();
{
	register int cc;

	key_fill();
	cc = is_delete_uniq(fd,buf);
	return(cc);
}
/* deletes a C-ISAM record  not using unique key */

delete(fd,key_fill)
int  fd;
int (*key_fill)();
{
	register int cc;

	key_fill();
	cc = is_delete(fd);
	return(cc);
}
