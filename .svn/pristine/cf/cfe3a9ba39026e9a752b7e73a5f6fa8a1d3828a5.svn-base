static char sz__FILE__[]="@(#)syncutils.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  syncutils.c; Rel: 10.1.2.1; Get date: 4/20/95 at 09:48:02
 * @(#)  Last delta: 12/2/94 at 18:46:38
 * @(#)  SCCS File: /taxi/sccs/s.syncutils.c
 *                                                                           *
 *****************************************************************************/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <errno.h>
#include "mads_types.h"
#include "mad_error.h"
#include "mads_isam.h"
#include "mad_ipc.h"
#include "df_maint.h"
#include "redncy.h"
#include "syncdb.h"
#include "path.h"

#define SYNC_SHM_START (char *)0xE40000  /* start address for shared memory */
#define NULL_PTR  (char *)0

extern struct stat *redncy_status;

int fd_trans = -1;                /* transaction file fd */
char trans_file[sizeof(TRANS_FILE)+2];
                                  /* the current transaction file name */
unsigned char file_alt = FILE_ALT0; /* trans file alternative (flip-flop) flag*/ /* this value is set in shared mem initially
                                     by init_shm_sync() */
int shm_addr = 0;                 /* starting shared memory addr for file state table */
int NOT_TO_SYNC[] =   /* files do not need to be synced */
   {
   SHIFT_FILE_ID,
   SHIFTH_FILE_ID
   };
                                  /* no. of not-to-be synced files */
int no_not_to_sync = sizeof(NOT_TO_SYNC)/sizeof(int);

static int begn_shm_addr = 0;     /* beginnig of the shared memory */
static int shmid = -1;                     /* shared memory id */

init_shm_sync(mode)
char mode;
{
	register int DBF_STATE_TBL_SZ;

/* calculate the size of shared memory:
    ===================================
    no. of transaction writes (int)
           (to the transaction file since last reset/swap
           (used for flip-flop on transaction file))
    -----------------------------------
    file_alt (unsigned char)
           (to indicate which file it is writing to)
    -----------------------------------
    state  (unsigned char) for file 1
    -----------------------------------
    state  (unsigned char) for file 2
    -----------------------------------
    ......
    ......
    -----------------------------------
    state  (unsigned char) for file n
    ===================================
*/


	DBF_STATE_TBL_SZ = TRANS_WR_CNT_SZ + TRANS_FILE_ALT_SZ + ENT_SZ_DBF_STATE_TBL * MAX_FILES + sizeof(struct stat);
	redncy_status = NULL;

	/* put db file state table in the shared memory, so that other processes
   	can access it.  other processes must not modify this table except the
   	SYNCDB and MIRROR processes */

  	shmid = shmget(SHM_KEY_SYNC, DBF_STATE_TBL_SZ, IPC_CREAT | 0666);

	if(shmid == -1) {
   		return(FAILED);
   	}

	/* lock the shared memory in core when created */

	if(mode == CREAT_SHM_INIT)
   		if(shmctl(shmid, SHM_LOCK, 0) < 0);
/*
      			ERROR(' ', "", "init_shm_sync: couldn't lock shared memory in core.");
*/

	/* attach the sync shared memory */

	if((begn_shm_addr = (int)shmat(shmid, 0, 0)) == -1) { /* put shared memory at fixed address */
   		sprintf(trace_str, "shmat failed - %d", errno);
   		ERROR(' ', "", trace_str);
   		return(FAILED);
   	}
/*
	if((char *)begn_shm_addr != SYNC_SHM_START) {								
   		ERROR(' ',"", "init_shm_sync: couldn't get shared memory address.");
   		return(FAILED);
   	}
*/

	/* set the status pointer to the beginning of redundancy shared memory	*/
	redncy_status = (struct stat *)(begn_shm_addr + TRANS_WR_CNT_SZ + TRANS_FILE_ALT_SZ + ENT_SZ_DBF_STATE_TBL * MAX_FILES);

	/* adjust for db file state table entry since the first field is the number of transaction writes */

	shm_addr = begn_shm_addr + TRANS_WR_CNT_SZ + TRANS_FILE_ALT_SZ; 

	/* if it is a shm creator, then set all files to be DONT_CARE */

	if(mode == CREAT_SHM_INIT) {
   		*((unsigned char *)begn_shm_addr + TRANS_WR_CNT_SZ) = file_alt;
   		init_sync_tbl(DONT_CARE);
   	}

	return(SUCCESS);
}

/*********************************************************/
/* set_sync(type, file_id)
/*    char type;
/*    int file_id;
/*
/*    return:
/*        0       successful
/*       -1       failed
/*
/*    inputs:
/*       type:    NOT_SYNCED, SYNCED, IN_SYNC, or DONT_CARE
/*       file_id: file id
/*
/*   It sets a database file to a desired state: SYNCED,
/*   IN_SYNC or NOT_SYNC, or DONT_CARE.
/*   It uses shared memory for state of MADS database files.
/*   Generally, this shared memory is created by MIRROR
/*   process.   If the shared memory has not been created
/*   this routine will fail.
/*********************************************************/
set_sync(type, file_id)
char type;              /* NOT_SYNCED, SYNCED, IN_SYNC, DONT_CARE */
int file_id;            /* this should be the same as MADS file id */
{
	unsigned char *cur_ent_addr;

	if(shm_addr == 0)
   		if(init_shm_sync(SUB_SHM_INIT) == FAILED)
      			return(FAILED);

	cur_ent_addr = (unsigned char *)shm_addr + file_id * ENT_SZ_DBF_STATE_TBL; /* get entry ptr for current file */

	switch(type) {
   		case NOT_SYNCED:        /* NOT SYNCED */
      			SET_NOT_SYNC(cur_ent_addr)
      			break;

   		case SYNCED:            /* SYNCED */
      			SET_SYNCED(cur_ent_addr)
      			break;

   		case IN_SYNC:           /* IN process of being SYNCED */
      			SET_IN_SYNC(cur_ent_addr)
      			break;

   		case DONT_CARE:         /* DON'T care, mirror any time */
      			SET_DONT_CARE(cur_ent_addr)
      			break;

   		default:
      			sprintf(trace_str, "set_sync: Unknown type - %x", type);
      			ERROR(' ', "", trace_str);
      			return(FAILED);
   	}
	return(SUCCESS);
}

/*********************************************************/
/* is_sync(file_id)
/*    int file_id;
/*
/*    return:
/*       -1       failed or unknown state
/*       NOT_SYNCED, SYNCED, IN_SYNC, DONT_CARE
/*   It checks if a database file has been SYNCED, IN_SYNC
/*   or NOT_SYNC or DONT_CARE depending on the type of
/*   request to check.
/*   It uses shared memory for state of MADS database files.
/*   Generally, this shared memory is created by MIRROR
/*   process.   If the shared memory has not been created
/*   this routine will fail.
/*********************************************************/

is_sync(file_id)
int file_id;            /* this should be the same as MADS file id */
{
	register int ret_val;
	unsigned char *cur_ent_addr;

	/* get handle on the shared memory */

	if(shm_addr == 0)
   		if(init_shm_sync(SUB_SHM_INIT) == FAILED)
      			return(FAILED);

	/* compute the entry address for this file */

	cur_ent_addr = (unsigned char *)shm_addr + file_id * ENT_SZ_DBF_STATE_TBL; /* get entry ptr for current file */

	/* find out which state */

	if(IS_SYNCED(cur_ent_addr))
   		ret_val = SYNCED;        /* SYNCED */
	else if(IS_IN_SYNC(cur_ent_addr))
      		ret_val = IN_SYNC;       /* IN process of being SYNCED */
   	else if(IS_NOT_SYNCED(cur_ent_addr))
         	ret_val = NOT_SYNCED;    /* NOT SYNCED */
      	else if(IS_DONT_CARE(cur_ent_addr))
            	ret_val = DONT_CARE;    /* DON'T CARE */
      	else  {                  /* Unknown state */
         	sprintf(trace_str, "is_sync: Unknown state for file %d\n", file_id);
         	ERROR(' ', "", trace_str);
         	ret_val = FAILED;
        }

	return(ret_val);
}

/*****************************************************/
/*  get_base_name(str)
/*
/*     Find a file base name from the full path file
/*     name.
/*
/*     return:
/*        string pointer to the base name
/*****************************************************/
char *
get_base_name(str) /* to find a basename from a full path file name */
char *str;
{
	int i,j=0;
	char c, tmp[FILE_NAME_LEN];

	while(((c = *str++) != '\0') && (c != '\t') && (c != ' '))
   		if (c == '/')
      			j=0;
   		else
      			tmp[j++] = c;

	tmp[j] = '\0';
	return(tmp);
}

/*****************************************************/
/*  rm_shm_sync()
/*
/*     Remove the shared memory (sync).
/*****************************************************/
rm_shm_sync() /* remove the shared memory (sync) */
{
	struct shmid_ds shmidds;

	/* detach the data segment */

	if(begn_shm_addr)
   		if (shmdt(begn_shm_addr) == -1) {
      			sprintf(trace_str, "Can't shmdt for %X - %d", begn_shm_addr, errno);
      			ERROR(' ', "", trace_str);
      		}

	/* remove the shared memory */

	if(shmid >=0) {
   		if (shmctl(shmid, IPC_STAT, &shmidds) == -1) {
      			sprintf(trace_str, "Can't get stat for shmid %d - %d", shmid, errno);
      			ERROR(' ', "", trace_str);
      			return(FAILED);
      		}

   		if(shmctl(shmid, IPC_RMID, &shmidds) == -1) {
      			sprintf(trace_str, "Can't remove shmid %d - %d", shmid, errno);
      			ERROR(' ', "", trace_str);
      			return(FAILED);
      		}
   	}
	return(SUCCESS);
}

/*****************************************************/
/*  init_sync_tbl(state)
/*
/*     Initialize the file state table in shared memory
/*     to the state desired.
/*****************************************************/
init_sync_tbl(state)
register char state;             /* SYNCED, IN_SYNC, NOT_SYNCED, DONT_CARE */
{
	register int i, j;

	if((state == DONT_CARE) || (state == SYNCED) || (state == IN_SYNC) || (state == NOT_SYNCED))
   		for (i=0; i < MAX_FILES; i++) {
      			set_sync(state, i);
      			for(j=0; j < no_not_to_sync; j++)
         			if(i == NOT_TO_SYNC[j]) {   /* set NOT-TO-BE synced file to NOT_SYNCED*/
            				set_sync(NOT_SYNCED, i);
            				break;
            			}
      		}
		else {
   			sprintf(trace_str, "init_sync_tbl: Can't init, Unknown state - %x\n", state);
   			ERROR(' ', "", trace_str);
   			return(FAILED);
   		}

	return(SUCCESS);
}

/*****************************************************/
/*  get_trans_wrts()
/*
/*     Get the number of transaction writes in the
/*     shared memory.  It is the first part of the
/*     shared memory.  This is used for swapping
/*     the transaction file in a flip-flop fashion.
/*
/*     4/4/88         by Jack Chou
/*****************************************************/
get_trans_wrts()
{
	if (begn_shm_addr == 0)    /* shared mem is NOT there, forget it assuming shm is init'ed before this is invoked */
    		return(FAILED);

	return(*(int *)begn_shm_addr);
}

/*****************************************************/
/*  set_trans_wrts(val)
/*
/*     Set the number of transaction writes in the
/*     shared memory to the desired value.  It is the
/*     first part of the shared memory.  This is used
/*     for swapping the transaction file in a flip-flop
/*     fashion.
/*
/*     4/4/88         by Jack Chou
/*****************************************************/
set_trans_wrts(val)
register int val;
{
	if(begn_shm_addr == 0)    /* shared mem is NOT there, forget it assuming shm is init'ed before this is invoked */
   		return(FAILED);

	*(int *)begn_shm_addr = val;

	return(SUCCESS);
}

/*****************************************************/
/*  get_file_alt()
/*
/*     Get the current transaction file alternative 
/*     in the shared memory.  It is the second part of
/*     the shared memory.  This tells you which is the
/*     transaction file used for transaction writes
/*     currently.
/*
/*     4/4/88         by Jack Chou
/*****************************************************/
unsigned char 
get_file_alt()
{
	register unsigned char cur_alt;

	if(begn_shm_addr == 0)    /* shared mem is NOT there, forget it assuming shm is init'ed before this is invoked */
   		return(0xFF);

	cur_alt = *((unsigned char *)begn_shm_addr+TRANS_WR_CNT_SZ);

	if(cur_alt != FILE_ALT0 && cur_alt != FILE_ALT1) {
   		ERROR(' ', "", "get_file_alt: file_alt out of range");
   		return(0xFF);
   	}

	return(cur_alt);
}

/*****************************************************/
/*  set_file_alt(val)
/*
/*     Set the current transaction file alternative 
/*     in the shared memory to the desired file.  It is
/*     the second part of the shared memory.  This is used
/*     for swapping the transaction file in a flip-flop
/*     fashion.
/*
/*     4/4/88         by Jack Chou
/*****************************************************/
set_file_alt(val)
register unsigned char val;
{
	if (begn_shm_addr == 0)    /* shared mem is NOT there, forget it assuming shm is init'ed before this is invoked */
   		return(FAILED);

	*((unsigned char *)begn_shm_addr+TRANS_WR_CNT_SZ) = val;

	return(SUCCESS);
}

/*****************************************************/
/*  swap_file()
/*
/*     This routine does the followings:
/*        1. open the transaction file if not opened yet.
/*        2. close the old file if already opened and
/*           open the alternative file.
/*        (don't need to change mode and owner, since
/*         that is done by open_trans_file().)
/*        3. reset the transaction writes count in the
/*           shared memory to 0.
/*
/*     4/4/88         by Jack Chou
/*****************************************************/
swap_file()
{
	if(fd_trans != -1) {
   		close(fd_trans);
   		file_alt ^= 0x01;
   	}

	set_file_alt(file_alt);

	if((fd_trans = open_trans_file('S')) == -1)
   		return(-1);

	set_trans_wrts(0);

	return(fd_trans);
}

/*****************************************************/
/*  get_trans_flnm()
/*
/*     This routine initializes the global, trans_file,
/*     to the desired name.
/*
/*     4/5/88         by Jack Chou
/*****************************************************/
char *
get_trans_flnm()
{
	register int len;
	unsigned char cur_alt;

	if((file_alt = get_file_alt()) == 0xFF)
   		return(NULL_PTR);

	cur_alt = file_alt ^ 0x01;
	strcpy(trans_file, TRANS_FILE);
	len = strlen(trans_file);
	trans_file[len++] = '0' + cur_alt;
	trans_file[len] = '\0';

	return(trans_file);
}

/*****************************************************/
/*  open_trans_file()
/*
/*     This routine does the followings:
/*        1. get the transaction file name.
/*        2. open the transaction file with selected mode.
/*        3. change file mode to the designated mode.
/*        4. change owner of the file to the designated.
/*
/*     4/5/88         by Jack Chou
/*****************************************************/
open_trans_file(mode)
register char mode;
{
	register int oflag;

	if(get_trans_flnm() == NULL_PTR) {
   		ERROR(' ', "", "Can't get trans file name; Init. sync shm first");
   		return(-1);
   	}

	switch(mode) {
   		case 'R':     /* read only (for mirror proc) */
      			oflag = O_RDONLY | O_CREAT;
      			break;
	
   		case 'I':     /* read and truncate (for mirror proc first open) */
      			oflag = O_RDONLY | O_CREAT | O_TRUNC;
      			break;

   		case 'A':     /* write only (for syncdb, synctime procs) */
      			oflag = O_WRONLY | O_APPEND;
      			break;

   		case 'S':     /* for swapping file */
      			oflag = O_WRONLY | O_APPEND | O_CREAT | O_TRUNC;
      			break;

   		default:
      			oflag = O_RDWR | O_APPEND | O_CREAT;
      			break;
   	}

	if((fd_trans = open(trans_file, oflag, 0660)) == -1) {
   		sprintf(trace_str, "Can't open %s - %d", trans_file, errno);
   		ERROR(' ', "", trace_str);
   		return(-1);
   	}

	if(oflag & O_CREAT) 
   		chg_mod(trans_file);

	return(fd_trans);
}

