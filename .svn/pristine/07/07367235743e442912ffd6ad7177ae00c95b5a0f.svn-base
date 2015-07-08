static char sz__FILE__[]="@(#)dbredncy.c	10.1.2.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  dbredncy.c; Rel: 10.1.2.1; Get date: 4/20/95 at 09:47:54
* @(#)  Last delta: 12/2/94 at 18:09:32
* @(#)  SCCS File: /taxi/sccs/s.dbredncy.c
*                                                                           *
*****************************************************************************/

#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include "mad_error.h"
#include "mads_types.h"
#include "mads_isam.h"
#include "redncy.h"
#include "syncdb.h"
#include "df_maint.h"

#ifdef TIMER
#include <sys/times.h>
struct tms tmptms;
long 
times(), startticks;
#endif

   extern struct file_desc isam_file[];
extern struct stat *redncy_status;

extern int fd_trans;		/* transaction file desc (for redundancy) */
/* init'ed in syncutils.c file */
extern unsigned char file_alt;	/* the transact file alternative currently
				 * being written to by other procs */
extern unsigned char get_file_alt();
static unsigned char cur_file_alt;	/* the transaction file this process
					 * is currently working on */
extern char req_buf[];
extern char buf[];

#ifdef TIMER
static int pid = 0;
#endif

/**********************************************************************
*   db_redncy(file_id, action_code)
*      int file_id, action_code
*
*   DESCRIPTION:        This is the main routine which shadows the
*                       database-updating (ADD_REC, UPDATE_REC or
*                       DELETE) activities on the database of the
*                       redundant (remote) machine by writing to the
*                       transaction file with proper information,
*                       including database type, action type, record,
*                       and data (see struct transact).
*
*   AUTHOR:             J.T. CHOU     4/28/88
*
*   PARAMETER LIST:     file_id       database type
*                       action_code   action type: ADD_REC, UPDATE_REC
*                                                  or DELETE
*
*   RETURNED ITEM:      None
*
*   PROCEDURES CALLED:
*        is_prim_redn   check if the system is the primary and in redundant
*                       state
*        is_synced      check if the database file can be shadowed
*        sema_oper      lock or unlock the transaction file by semaphore
*        open_trans_file   open the transaction file
*        get_trans_wrts get number of transactions in the current
*                       transaction file
*        db_trans_wrt   write a transaction to the current transaction file
*
*   GLOBALS:
*        fp_trans       transaction file pointer
*        file_alt       current designated file alternative
*        cur_file_alt   current opened file alternative (static)
*
*   IMPLEMENTATION:
*        It shadows or mirrors  the database updating activities (ADD_REC,
*        UPDATE_REC, or DELETE) only when:
*           1. it is the primary host
*           2. it is in redundant state
*           3. the database file can be shadowed, i.e., the file
*              is either in SYNCED or in IN_SYNCED or in DON'T CARE state.
*
*        Then it writes the transaction to the transaction file with the
*        defined information.
*
*        4/28/88  Jack Chou   Add trans file alternating scheme:
*
*           It locks the transaction file during the shadowing activities:
*              1. open the transaction file (if not opened yet)
*              2. check if the transaction file needed to be swapped
*                 (if the transaction file has the defined number of
*                  transactions then transaction file is swapped.)
*              3. write the transaction to the transaction file
*
*           Then the transaction file is unlocked.
*
*           The UNIX Semaphore is used for this lock/unlock machanism.
*
*           The current transaction file and number of transactions in the
*           file are in the sync shared memory for IPC among different
*           processors. (for details, please see syncutils.c file)
*
*           THIS is the ONLY routine which does the transaction file
*           swapping.
*
*   CONSTRAINTS AND
*   ERROR CONDITIONS:
*
*   HISTORY:
*        2/8/88  Jack Chou  Initial. This used to be part of db().
*        4/28/88 Jack Chou  Add trans file alternating scheme.
***********************************************************************/

db_redncy(file_id, action_code)
   int   action_code, file_id;
{
   register int val;
   register short shm_locked = 0;	/* sync shared memory lock flag */

   if (file_id == ERROR_FILE_ID)
      return;

   if (redncy_status == NULL )
   	init_shm_sync(SUB_SHM_INIT);

   if (is_prim_redncy())	/* primary and reduncy */
   {
      if (file_id >= 0)		/* MRB */
	 val = is_sync(file_id);/* is the file already synced ? */
      else			/* MRB */
	 val = SYNCED;		/* MRB */
      if ((val == SYNCED) || (val == IN_SYNC) || (val == DONT_CARE))
	 /*
	  * file being synced or in the process of being synced or don't care
	  * can be mirrored
	  */
      {
#ifdef TIMER
	 startticks = times(&tmptms);
	 if (pid == 0)
	    pid = getpid();
#endif
	 if (!shm_locked)
	    if (sema_oper(-1) == 0)	/* lock the shared mem */
	    {
	       shm_locked = 1;
#ifdef TIMER
	       sprintf(timer_str, "pid= %d: db: lock sync shm at %d\n", pid, times(&tmptms));
	       TIMER_T(timer_str);
#endif
	    } else
	       return;

	 /* if trans file is not opened yet then open it */

	 if (fd_trans == -1)
	 {
	    if ((fd_trans = open_trans_file('A')) == -1)
	    {
	       sprintf(trace_str, "Can't open trans file - %d", errno);
	       ERROR(' ', "", trace_str);
	       if (shm_locked)
	       {
		  sema_oper(1);	/* unlock shared mem if locked */
		  shm_locked = 0;
#ifdef TIMER
		  sprintf(timer_str, "pid= %d: db: unlock sync shm at %d\n", pid, times(&tmptms));
		  TIMER_T(timer_str);
#endif
	       }
	       return;
	    }
	    cur_file_alt = file_alt;	/* keep track of trans file */
	 }
	 /* check potential file swapping */

	 if (get_trans_wrts() >= FLIP_FLOP_LMT)
	 {			/* open alternative trans file if reach the
				 * limit */
	    /****************************************/
	    /*
	     * This is the only place where does the /* transaction file
	     * swapping (flip-flop). /***************************************
	     */
	    if ((fd_trans = swap_file()) == -1)
	    {
	       if (shm_locked)
	       {
		  sema_oper(1);	/* unlock shared mem */
		  shm_locked = 0;
#ifdef TIMER
		  sprintf(timer_str, "pid= %d: db: unlock sync shm at %d\n", pid, times(&tmptms));
		  TIMER_T(timer_str);
#endif
	       }
	       return;
	    }
	    cur_file_alt = file_alt;	/* keep track of trans file */
	 }
	 db_trans_wrt(file_id, action_code);

	 if (shm_locked)
	 {
	    sema_oper(1);	/* unlock shared mem if locked */
	    shm_locked = 0;
#ifdef TIMER
	    sprintf(timer_str, "pid= %d: db: unlock sync shm at %d\n", pid, times(&tmptms));
	    TIMER_T(timer_str);
#endif
	 }
#ifdef TIMER
	 sprintf(timer_str, "pid= %d: db: Ticks used for act %d - %d \n", pid,
		 action_code, times(&tmptms) - startticks);
	 TIMER_T(timer_str);
#endif
      }
   }
   return;
}				/* end db_redncy */

/**********************************************************************
*   db_trans_wrt(file_id, action_code)
*      int file_id, action_code
*
*   DESCRIPTION:        This routine updates the redundant (remote)
*                       database by writing to the transaction file
*                       with proper information, including database
*                       type, record, and data (see struct transact).
*
*   AUTHOR:             J.T. CHOU     2/8/88
*
*   PARAMETER LIST:     file_id       database type
*                       action_code   action type: ADD_REC, UPDATE_REC
*                                                  or DELETE
*
*   RETURNED ITEM:      None
*
*   PROCEDURES CALLED:
*        oepn           open a file
*        write          write to a file
*        memcpy         copy (n bytes) from one memory area to another
*        get_file_alt   get current file alternative
*        open_trans_file open transaction file
*        get_trans_wrts get current number of transaction writes
*        set_trans_wrts set number of transaction writes in shared memory
*
*   GLOBALS:
*        req_buf        buffer for from-isam conversion
*        isam_file      isam file description table
*        fp_trans       transaction file pointer
*        isrecnum       current isam file record number
*        file_alt       current designated file alternative
*        cur_file_alt   current opened file alternative (static)
*
*   IMPLEMENTATION:
*        4/28/88
*        It writes transaction to the current transaction file.
*        Since there are many processors (ui's) write to the same
*        file and flip-flop scheme is used for alternating the file
*        The current file is checked before writing.
*
*   CONSTRAINTS AND
*   ERROR CONDITIONS:
*
*   HISTORY:
***********************************************************************/

db_trans_wrt(file_id, action_code)
   register int file_id, action_code;
{
   register int data_sz;
   struct transact transact;

   if (file_id >= 0)
      data_sz = isam_file[file_id].size;

   transact.file = file_id;

   switch (action_code)
   {
   case UPDATE_REC:
      transact.action = REC_UPDATE;
      transact.rec_no = isrecnum;
      break;

   case ADD_REC:
      transact.action = REC_ADD;
      transact.rec_no = 0;
      break;

   case DELETE:
      transact.action = REC_DELETE;
      transact.rec_no = isrecnum;
      break;

   case FILE_UPDATE:		/* MRB */
      transact.action = FILE_UPDATE;
      transact.rec_no = 0;
      break;

   default:
      return;
   }


   if (file_id >= 0)
   {
      transact.size = TRANS_HDR_SIZE + data_sz;
      memcpy(transact.data, req_buf, data_sz);
   } else
   {
      transact.size = TRANS_HDR_SIZE + strlen(req_buf);
      strcpy(transact.data, req_buf);
   }


   /* check if the trans file got changed (before writing) */

   if ((file_alt = get_file_alt()) == 0xFF)
   {
      ERROR(' ', "", "Can't determine trans file alternative");
      return;
   }
   if (file_alt != cur_file_alt)/* file got changed */
   {
      close(fd_trans);
      if ((fd_trans = open_trans_file('A')) == -1)
      {
	 ERROR(' ', "", "Can't open trans file");
	 return;
      }
      cur_file_alt = file_alt;
   }
   if (write(fd_trans, (char *) &transact, transact.size) < 0)
   {
      sprintf(trace_str, "Can't write to trans file - %d", errno);
      ERROR(' ', "", trace_str);
      return;
   }
   set_trans_wrts(get_trans_wrts() + 1);	/* update the writes */
}				/* end db_trans_wrt */
