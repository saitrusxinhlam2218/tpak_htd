static char sz__FILE__[]="@(#)pi_act_init.c  10.1.2.1(ver) Date Release 12/2/94";
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  pi_act_init.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:45:32
 * @(#)  Last delta: 12/2/94 at 18:35:46
 * @(#)  SCCS File: /taxi/sccs/s.pi_act_init.c
 *                                                                           *
 *****************************************************************************/
/*------------------------------------------------------------------------------- 
   Copyright 1993 - TaxiCom Copenhagen, Denmark

   System	:	PLANET - LINK 
   Filename	:	pi_act_init.c
   Purpose	:	This module initializes the interface to TaxiPak.
			(DB, shared memory ... )

Modification history

Author	Date	Revision	Description
------  ------- --------------- -----------------------------------------------
LG	940126 	0.1		Creation of Shared-memory/db access.
------------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <isam.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include "taxi_db.h"
#include "mad_ipc.h"
#include "mads_types.h"
#include "menu.h"
#include "user.h"
#include "calls.h"
#include "except.h"
#include "ui_def.h"
#include "key.h"
#include "language.h"
#include "enhance.h"
#include "switch_ext.h"
#include "pi.h"

/* global variables */

extern int ListenSock;
int global_msg_num;	/* for CALL_W_MSG exception */
long glEnhSwitch; /* Enhancement switch for enhancement checking */

struct excep {					/* The rest is for the simulator as well.	*/
  int excep_type;				/* Internal exception-type enumrations.		*/
  long last_nbr;				/* Number of the last exception reported.	*/
  char on_off;					/* ON=1/OFF=0.					*/
  } ; 

struct excep_mess {
  long last_nbr;				/* Number of the last message-exception reported.*/
  char on_off;					/* ON=1/OFF=0.					*/
  } ; 

struct excep on_off_excep[NUM_OF_EXCEP_TYPE ];			/* Table of exceptions to report.		*/
struct excep_mess mess_on_off[ NUM_OF_EXCEP_MESS ];		/* Message to report (ON) or not (OFF).		*/
struct excep_mess drv_mess_on_off[ NUM_OF_EXCEP_MESS ];	/* Driver-message to report (ON) or not (OFF).	*/

extern int iserrno;				/* cisam errno */
short mem_attached = 0;				/* 0=shared memory not attached; 1=it is attached; is used by cleanup() */
short curses_set=0;				/* =1,if curses is initilzied,else 0; cleanup() looks at it */
int scr_name;					/* name of screen on display */
int fifo_fd;					/* file descriptor for opening FIFO */
int msgkey;					/* for storing msg queue id */
int shmid;					/* for saving shared memory segment id */
int semid;					/* for saving semaphore id */
struct fleets *fleet[FLEET_MAX];		/* array of pointers to point to fleet structures in shared memory */
struct offsets *offset;				/* pointer to point to the offset structure in shared memory */
struct users *uiuser[USER_MAX];			/* array of pointers to point to users structure in shared memory */
struct menus *menu[MENU_MAX];			/* array of pointers to point to menus  structure in shared memory */
int uid = 9999;					/* users id as in /etc/passwd */
int pid;					/* process id of this interface process */
int user_index;				/* index to be used by this user for the users structure in structure in shared memory */
extern char req_buf[];				/* buffer for reading/writing cisam records */
char *A_shm_ptr;					/* pointer to beginning of shared memory segment, as returned  by shmat() */
extern int errno;				/* system generated errno */
extern char *sys_errlist[];
extern int (*security[])();			/* array of pointers to functions for checking users permissions */
FILE *trace_fp; 				/* pointer to open trace file */
char canmtxt[MAX_CANM+1][41];			/* array to store canned mesg text */
int	basecall;
extern int def_fl_index;
extern int No_time_update;
int Dispatch_pid;
void cleanup();

struct exception_count_struct *exception_count;
char exception_group_desc[EXC_GROUPS][8];

/* ----------------------------------------------------------------------------
/* FUNCTION: INIT_ACTION
/*
/* DESCRIPTION: Initializes PI.
/*
/* PARAMETER-LIST: CON.
/* 
/* RETURN-VALUE: INT.
/*
------------------------------------------------------------------------------ */

int init_action(void)
  {
    int i;

      for ( i = 0; i < NUM_OF_EXCEP_TYPE; i++ )
        {
          on_off_excep[ i ].on_off = SWITCH_OFF;
          on_off_excep[ i ].last_nbr = 0;
        }

      for ( i = 0; i < NUM_OF_EXCEP_MESS; i++ )
        {
          mess_on_off[ i ].on_off = SWITCH_OFF;
          mess_on_off[ i ].last_nbr = 0;
          drv_mess_on_off[ i ].on_off = SWITCH_OFF;
          drv_mess_on_off[ i ].last_nbr = 0;
        }
    
    if ( !(glEnhSwitch & ENH0002) )
      {
	mess_on_off[12].on_off = 1;
	mess_on_off[4].on_off = 1;
      }

      on_off_excep[ 0 ].excep_type = EMERGENCY;
      on_off_excep[ 1 ].excep_type = CALL_CANCEL;
      on_off_excep[ 2 ].excep_type = UNZONED;
      on_off_excep[ 3 ].excep_type = EXTRA_TAXIS;
      on_off_excep[ 4 ].excep_type = CALLBACK;
      on_off_excep[ 5 ].excep_type = CALL_TIMEOUT;
      on_off_excep[ 6 ].excep_type = LATE_METER;
      on_off_excep[ 7 ].excep_type = FLAG_REQ;
      on_off_excep[ 8 ].excep_type = NO_SHOW;
      if ( !(glEnhSwitch & ENH0002) )
	on_off_excep[ 8 ].on_off = 1;
      on_off_excep[ 9 ].excep_type = CALLOUT;
      on_off_excep[ 10 ].excep_type = REQ_TO_TLK;
      on_off_excep[ 11 ].excep_type = MESSAGE;

      if ( !(glEnhSwitch & ENH0002) )
	on_off_excep[ 11 ].on_off = 1;

      on_off_excep[ 12 ].excep_type = DRV_MSG;
      on_off_excep[ 13 ].excep_type = SHORT_METER;
      on_off_excep[ 14 ].excep_type = SYS_ERR;
      on_off_excep[ 15 ].excep_type = MFAULT;
      if ( !(glEnhSwitch & ENH0002) )
	on_off_excep[ 15 ].on_off = 1;
			on_off_excep[ 16 ].excep_type = MISSING_RECEIPT_RETRY_LIMIT;
			on_off_excep[ 17 ].excep_type = SP_EXCPT_01;
			on_off_excep[ 18 ].excep_type = SP_EXCPT_02;
			on_off_excep[ 19 ].excep_type = SP_EXCPT_03;
			on_off_excep[ 20 ].excep_type = SP_EXCPT_04;
			on_off_excep[ 21 ].excep_type = SP_EXCPT_05;
			on_off_excep[ 22 ].excep_type = SP_EXCPT_06;
			on_off_excep[ 23 ].excep_type = SP_EXCPT_07;
			on_off_excep[ 24 ].excep_type = SP_EXCPT_08;
			on_off_excep[ 25 ].excep_type = SP_EXCPT_09;
			on_off_excep[ 26 ].excep_type = SP_EXCPT_10;
			on_off_excep[ 27 ].excep_type = SP_EXCPT_11;
			on_off_excep[ 28 ].excep_type = SP_EXCPT_12;
			on_off_excep[ 29 ].excep_type = SP_EXCPT_13;
			on_off_excep[ 30 ].excep_type = SP_EXCPT_14;
			on_off_excep[ 31 ].excep_type = SP_EXCPT_15;


  } /* end init_action */ 

#ifdef DB_PROFILE
#include "db_prof.h"

/* ----------------------------------------------------------------------------
/* FUNCTION: GET_DB_SHM
/*
/* DESCRIPTION: Routine to attach to the shared memory used to track UI 
/*		database activity. The stats are logged by db() to a shared 
/*		memory segment set up for ui the same stats are kept for dispatch 
/*		in a different memory segment.
/*
/* PARAMETER-LIST: NONE.
/* 
/* RETURN-VALUE: DUMMY.
/*
------------------------------------------------------------------------------ */
get_db_shm()
{
	int dbshm_size;

	/* determine the size of the shared memory segment to be used */
	dbshm_size = sizeof( unsigned int ) * N_DB_FILES * N_DB_ACTIONS;

	/* get the id for the shared memory segment to be attached */
	if ( (db_shmid = shmget(DBU_SHM_KEY,dbshm_size,0666)) == -1 )
	{
		db_shmid  = 0;
		db_shmptr = NULL;
		return;
	}

	/* attach the shared memory segment */
	if ( (db_shmptr = (unsigned int (*)[][N_DB_ACTIONS])shmat(db_shmid,0,0)) <= 0 )  
	{
		db_shmptr = NULL;
		return;
	}
}
#endif

/* ============================================================================================================================	*
 * init: 
 * 
 *	Returns: 
 *	      	 
 * ============================================================================================================================	*/

/* ----------------------------------------------------------------------------
/* FUNCTION: INIT_DISP
/*
/* DESCRIPTION: Initializing routine for the user interface program. 
/*		It opens various files requied by the program during run time. 	
/*		The function also initializes various global data structure 
/*		used during the life time of the program.
/*
/* PARAMETER-LIST: NONE.
/* 
/* RETURN-VALUE: DONE 		- if initialization is completed successfully.
/*		 NOT_DONE 	- if an error occurs in initialization.
------------------------------------------------------------------------------ */

init_disp()
{
	int addr;			/* address where shared memory should be attached */
	int seg_size;			/* size of shared memory segment to be attached */
	int i,j;			/* scratch variable */
	struct cisam_up *user_ptr; 	/* pointer to cisam_up structure for reading user profile record from cisam file on disk */
	void display_time();
	int check_if_dispatch_is_up();
	char ch_nbr;			/* for accessing the attributes file */
	char time_str[7];
	char term_name[32];
	struct cisam_at *attr_ptr;	/* to access attributes file */
	struct cisam_cm *cm_ptr;	/* to access canned mesg file */
	struct cisam_cl *cl_ptr;	/* to access call file */
	struct cisam_sl *sl_ptr;	/* pointer to seal file */
	struct cisam_eg *eg_ptr;
	int read_flag;			/* Stores status for db() read */
	void emergency_notify();
	struct users *uiptr;

	/* initialize signal catchers */

	/* If the environment variable UI_SIG is set, an interupt will
	   shut down UI and it's possible to send a stop or a kill from
	   the keyboard. Otherwise if UI_SIG is not set, those signals
	   are ignored. */
	if (getenv("UI_SIG"))
	  {
	    signal(SIGINT, cleanup);
	  }
	signal(SIGHUP, cleanup);
	signal(SIGPIPE, cleanup); /*occurs if writing to pipe with no dispatch*/
	signal(SIGTERM, cleanup);
	signal(SIGUSR1, cleanup);

	/*
	 * call routines to initialize all the string arrays required
	 */

        MakeLogEntry("start of init_disp");
	if (check_if_dispatch_is_up() == -1)
	  {
	    MakeLogEntry( "Cannot start. Dispatch is not up.");
            cleanup();
	    close(ListenSock);
	    exit(-1);
	  }


	    

#ifdef DEBUG
	/* open the trace file to be used by the user interface program */
	if ( (trace_fp = fopen(TRACE_FILE,"a")) == NULL ) {
		/* cannot open trace file */
		ERROR( "Cannot open TRACE-FILE." );
           cleanup();
            return -1;
	}  /* end if */

	/* Make trace file world readable and writable */
	chmod(TRACE_FILE, "00666");
#endif

	/* open FIFO to write requests to dispatch process */
	if((fifo_fd = open(USER_PIPE, O_RDWR | O_NDELAY)) == -1)  {
		/* cannot open FIFO */
		MakeLogEntry( "Cannot open FIFO." );
           cleanup();
            return -1;
	}

	/* get id of the msg queue used to pass messages between the user interface and the dispatch program */
	if ( (msgkey = msgget(PI_MSGKEY,IPC_CREAT|0666)) == -1 )  {
		/* msgget failed */
		MakeLogEntry( "Cannot open MESSAGE-QUEUE." );
           cleanup();
            return -1;
	}  /* end msgget */

	/* determine the size of the shared memory segment to be used */
	seg_size = sizeof(struct offsets) + FLEET_MAX*sizeof(struct fleets) + CALLS_MAX*sizeof(struct calls) + 
	    USER_MAX*sizeof(struct users) + EXCEPT_MAX*sizeof(struct excepts) + MENU_MAX*sizeof(struct menus) +
	    EXCEPT_DESC_MAX * sizeof(struct excpt_desc)
	      + sizeof(struct exception_count_struct);

	/* get the id for the shared memory segment to be attached */
	if ( (shmid = shmget(SHMKEY,1603596,0666)) == -1 )  {
		/* shmget failed */
		MakeLogEntry( "Cannot access SHARED-MEMORY." );
           cleanup();
            return -1;
	}  /* end shmget */

	/* attach the shared memory segment */
	sprintf(szLogMsg, "shmid %d seg_size %d", shmid, seg_size);
	MakeLogEntry(szLogMsg);
        if ( (A_shm_ptr = shmat(shmid, (const void *)0x0a00000010000000, 0)) == (char *)-1 )  {	
		/* shmat failed */
	  sprintf(szLogMsg, "Cannot attach SHARED-MEMORY %d %d", shmid, errno);
		MakeLogEntry( szLogMsg );
           cleanup();
            return -1;
	}  /* end if shmat */

	/* shmat worked; set the mem_attached which cleanup() looks at */
	mem_attached = 1;

#ifdef DB_PROFILE
	get_db_shm();
#endif


	MakeLogEntry("After shm attach");
	/* get semaphore id to be used to lock the array of excepts in  shared memory. Note that "Dispatch" should 
	 * create the set of 2 semaphores - a read semaphore and a write semaphore
	 */
	if((semid = semget(SEMKEY,NSEMS,0666|IPC_CREAT)) == -1)  {
		/* semget failed */
		MakeLogEntry( "Cannot get SHARED-MEMORY semaphores." );
           cleanup();
            return -1;
	} 

	/* initialize pointers for accessing structures/variables in shared memory segment */

	/* initialize the pointer to point to the offsets structure in shared memory */
	offset  = (struct offsets *)A_shm_ptr;

	MakeLogEntry("offset dispatch check");
	i = 0;
	if ( !offset->status.dispatch )
	  {
	    /** Dispatch is running but not yet initialized **/
	    while ( ( !offset->status.dispatch ) && ( i < 10 ) )
	      {
		sleep( 5 );
		i++;
	      }
	  }
	
	if ( i >= 10 )
	  {
	    cleanup();
	    return -1;
	  }

	/* Exception count is at end of shared memory (need to make this less kludgy) */
	exception_count = (struct exception_count_struct *) offset->except_count;

	if (db(EXC_GROUP_ID, OPEN, &eg_key1, ISINOUT + ISMANULOCK, 0) < 0)
	  {
	    MakeLogEntry("Error opening exception-group file");
           cleanup();
            return -1;
	  }
	
	eg_ptr = (struct cisam_eg *) req_buf;
	for (i = 1; i <= EXC_GROUPS; i ++)
	  {
	    eg_ptr->nbr = i;						
	    if (db(EXC_GROUP_ID, READ_KEYS, &eg_key1, ISEQUAL, 0) < 0)
	      exception_group_desc[i - 1][0] = '\0';
	    else
	      strncpy(exception_group_desc[i - 1], eg_ptr->name, 8);
	  }
	
	if (db(EXC_GROUP_ID, CLOSE, &eg_key1, 0, 0) < 0)
	  {
	    MakeLogEntry("Error opening exception-group file");
           cleanup();
            return -1;
	  }

	/* initialize pointers to fleet structure in shared memory */
	fleet[0] = offset->fleet;
	for (i = 1; i < FLEET_MAX; i++)
		fleet[i] = fleet[0] + i;

	/* initialize pointers to users structure in shared memory */
	uiptr = offset->user;
	uiuser[0] = uiptr++;
	for(i = 1; i < USER_MAX; i++ )
		uiuser[i] = uiptr++;

	/* initialize pointers to menu structure in shared memory */
	menu[0] = offset->menu;
	for (i = 1; i < MENU_MAX; i++ )
		menu[i] = menu[0] + i;

	/* get the process id # */
	pid = getpid();

	if ((user_index = get_user_index()) == -1)
          {
	    sleep(20);  /** Delay a moment to give dispatch a chance to start **/
	    if ((user_index = get_user_index()) == -1)
	      {
		cleanup();
		return -1;
	      }
          }

	/* Immediately fill user data for pid and uid so no
	   other UI can grab it */
	uiuser[user_index]->p_id = pid;
	uiuser[user_index]->user_id = uid;

	user_ptr = (struct cisam_up *)req_buf;
	user_ptr->user_id = (short)uid;

/*  This is done by the ERROR macro in common/error.c:mad_error()
open the error file on disk
	if(db(ERROR_FILE_ID,OPEN,&el_key1,ISINOUT+ISMANULOCK,0) < 0) {
	    ERROR("Error opening error-log file");
            cleanup(); 
            return -1;
	}
*/

	/* fill the appropriate portion of the shared memory segment with the users information from user profile */
	fill_user();

    /* get the count of messages for the user */
    uiuser[user_index]->cnt_msg = (short)message_count(uiuser[user_index]->user_id);

	/* open the call file on disk */
	if(db(CALL_FILE_ID,OPEN,&cl_key1,ISINOUT+ISMANULOCK,0) < 0) {
		/* db failed to read record from disk */
	    MakeLogEntry("DB failed to read record from disk");
           cleanup();
            return -1;
	} 

	cl_ptr = (struct cisam_cl *)req_buf;
	if(db(CALL_FILE_ID, READ_KEYS, &cl_key1, ISLAST, 0) < 0) 
		basecall = 0;
	else
		basecall = cl_ptr->nbr;

	/* open the call_msg table */
	if(db(CALLMSG_FILE_ID,OPEN,&cl_key1,ISINOUT+ISMANULOCK,0) < 0) {
		/* db failed to read record from disk */
			MakeLogEntry("DB failed to open CALL_MSGS table");
					 cleanup();
						return -1;
	}


	/* open the call history file on disk */
	if(db(CALLH_FILE_ID,OPEN,&ch_key1,ISINOUT+ISMANULOCK,0) < 0) {
		/* db failed to open call history file */
	    MakeLogEntry("DB failed to open call history file");
           cleanup();
            return -1;
	}

	/* open the address file on disk */
	if(db(ADDRESS_FILE_ID,OPEN,&ad_key1,ISINOUT+ISMANULOCK,0) < 0) {
		/* db failed to read record from disk */
	    MakeLogEntry("DB failed to open address file");
           cleanup();
            return -1;
	}

	/* open the subzone file on disk */
	if(db(SUBZONE_FILE_ID,OPEN,&sz_key1,ISINOUT+ISMANULOCK,0) < 0) {
		/* db failed to read record from disk */
	    MakeLogEntry("DB failed to open subzone file");
           cleanup();
            return -1;
	}

	/* open the street name file on disk */
	if(db(STRNAME_FILE_ID,OPEN,&sn_key1,ISINOUT+ISMANULOCK,0) < 0) {
		/* db failed to read record from disk */
	    ERROR("DB failed to open streetname file");
           cleanup();
            return -1;
	}

	/* open the street type file on disk */
	if(db(STRTYPE_FILE_ID,OPEN,&st_key1,ISINOUT+ISMANULOCK,0) < 0) {
		/* db failed to open street type file on disk */
	    ERROR("DB failed to open streettype file");
           cleanup();
            return -1;
	}

	/* open the locality file on disk */
	if(db(LOCALTY_FILE_ID,OPEN,&lo_key1,ISINOUT+ISMANULOCK,0) < 0) {
		/* db failed to open locality file on disk */
	    ERROR("DB failed to open locality file");
           cleanup();
            return -1;
	}

	/* open the place name file on disk */
	if(db(PLACENM_FILE_ID,OPEN,&pl_key1,ISINOUT+ISMANULOCK,0) < 0) {
		/* db failed to open place name file on disk */
	    ERROR("DB failed to open place-name file");
           cleanup();
            return -1;
	}

	/* open the zone file on disk */
	if(db(ZONE_FILE_ID,OPEN,&zn_key1,ISINOUT+ISMANULOCK,0) < 0) {
		/* db failed to open zone file on disk */
	    ERROR("DB failed to open zone file");
           cleanup();
            return -1;
	} 

	/* open the vehicle file on disk */
	if(db(VEHICLE_FILE_ID,OPEN,&veh_key1,ISINOUT+ISMANULOCK,0) < 0) {
		/* db failed to open vehicle file on disk */
	    ERROR("DB failed to open vehicle file");
           cleanup();
            return -1;
	} 

	/* open the driver file on disk */
	if(db(DRIVER_FILE_ID,OPEN,&dr_key1,ISINOUT+ISMANULOCK,0) < 0) {
		/* db failed to open driver file on disk */
	    ERROR("DB failed to open driver file");
           cleanup();
            return -1;
	}

	/* open the statusr file on disk */
	if(db(STATUSR_FILE_ID,OPEN,&su_key1,ISINOUT+ISMANULOCK,0) < 0) {
		/* db failed to status-r file on disk */
	    ERROR("DB failed to open statusr file");
           cleanup();
            return -1;
	}

	cm_ptr = (struct cisam_cm *)req_buf;
	/* set up canned message text array for messages */
	if(db(CANMSG_FILE_ID,OPEN,&cm_key1,ISINOUT+ISMANULOCK,0) < 0) {
		/* db failed to open canmsg file */
	    ERROR("DB failed to open canmsg file");
           cleanup();
            return -1;
	}

	/* read records and store text in array */
	if (db(CANMSG_FILE_ID,READ_KEYS,&cm_key1,ISFIRST,0) >= 0) 
          {
	    for(i=0; i==0; ) 
              {
	        if (cm_ptr->nbr > MAX_CANM)
                  {
	            ERROR("Canned message out of range");
                  }
	        else
                  {
		    strncpy(&canmtxt[cm_ptr->nbr][0],cm_ptr->text,40);
		    if (db(CANMSG_FILE_ID,READ_KEYS,&cm_key1,ISNEXT,0) < 0)
		      i++;
                   }
		}
	     }

	if (db(CANMSG_FILE_ID,CLOSE,&cm_key1,0,0) < 0) {
		/* db failed to close file */
	    ERROR("DB failed to close canmsg file");
           cleanup();
            return -1;
	}

	db( CUSTOMER_FILE_ID, OPEN, &cust_key1, ISINOUT + ISMANULOCK, 0 );


        db_open(GEOADDR_ADDR_FILE_ID, ISINOUT+ISMANULOCK);
        db_open(GEOADDR_LINE_FILE_ID, ISINOUT+ISMANULOCK);
        db_open(GEOADDR_POINT_FILE_ID, ISINOUT+ISMANULOCK);
        
	/* If somehow another UI grabbed the user memory slot that we
	   did, exit this UI and tell the other UI to exit. */
	if (uiuser[user_index]->user_id != uid) {
	  ERROR("User memory conflict. Please restart.");

	    /* Set the other UI's pid to 0 so to signify that the other UI's
	       status line is changed so user is aware of problem. */
	    uiuser[user_index]->p_id = 0;
            cleanup();
            return -1;
	}
	return(DONE);

}  /* end init_disp()  */

/* ----------------------------------------------------------------------------
/* FUNCTION: CHECK_IF_DISPATCH_IS_UP
/*
/* DESCRIPTION: This routine checks if the dispatch-process is up.
/*
/* PARAMETER-LIST: NONE.
/* 
/* RETURN-VALUE: 
/*		
------------------------------------------------------------------------------ */

check_if_dispatch_is_up()
{
  FILE *fd;
  int  rc;

  if ((fd = fopen(DISPATCH_PID_FILE, "r")) == NULL)
    return(-1);

  fscanf(fd, "%d", &Dispatch_pid);
  fclose(fd);

  /* Check if dispatch PID is still up. kill(pid, 0) returns
     a -1 and an errno of ESRCH if the PID does not exist */
  rc = kill(Dispatch_pid, 0);
  if (rc == -1 && errno == ESRCH)
    return(-1);
  return(0);
} /* end check_if_dispatch_is_up()  */

/* ----------------------------------------------------------------------------
/* FUNCTION: CLOSE_ALL_DB_FILES
/*
/* DESCRIPTION: This routine closes all the file-descriptors (DB).
/*
/* PARAMETER-LIST: NONE.
/* 
/* RETURN-VALUE: NONE.
/*		
------------------------------------------------------------------------------ */

close_all_db_files()
{
  int  i;

  /* Close any open db files */
  for (i = 0; i < DB_NUM_FILES; i ++)
    if (Db_files[i].fd != -1)
      {
	if (db(i, CLOSE, 0, 0, 0) < 0)
	  ERROR("DB-file cannot be closed.");
      }
}/* end close_all_db_files()  */

/* ----------------------------------------------------------------------------
/* FUNCTION: CLEANUP
/*
/* DESCRIPTION: This is the PI-cleanup function.
/*
/* PARAMETER-LIST: NONE.
/* 
/* RETURN-VALUE: 
/*		
------------------------------------------------------------------------------ */

void cleanup()
{
	char shell_command[80];	/* buffer to save logo file name to cat */
	int i;

#ifdef DEBUG
	if (trace_fp)
	    fclose(trace_fp);
#endif

	close(fifo_fd);

	/* Erase pid and uid reference. This is to mark the user memory slot as
	   available again */

	if (uiuser[user_index])
	  {
	    uiuser[user_index]->p_id = 0;
	    uiuser[user_index]->user_id = 0;
	  }

	if ( mem_attached ) { 				/* reset variables in users structure for this user in shared memory */
		uiuser[user_index]->user_id = 0;
		uiuser[user_index]->p_id = 0;		/* this tells the counter process, that the user logged out */
		shmdt(A_shm_ptr); 			/* detach shared memory */
	}  /* end if mem_attached */

	close_all_db_files();

}/* end cleanup()  */

/* ----------------------------------------------------------------------------
/* FUNCTION: GET_USER_INDEX
/*
/* DESCRIPTION: This function finds an empty slot in the user space of
/* 		shared memory for data to be stored for the UI. An empty
/* 		slot should have a pid and uid of zero if the slot has
/* 		never been used or the slot has been used and the UI that was
/* 		using exited cleanly. A random slot is picked and if that
/* 		slot is already taken, a linear search is done to find the
/* 		first open one. If it can't find an empty slot, it will check
/* 		to see if the pids listed in the slots are still active (they
/* 		not be active if a UI died) and chose the first available slot. 
/*
/* PARAMETER-LIST: NONE.
/* 
/* RETURN-VALUE: NONE.
/*		
------------------------------------------------------------------------------ */

get_user_index()
{
    int i, start_point, rc;

    /* This function finds an empty slot in the user space of
       shared memory for data to be stored for the UI. An empty
       slot should have a pid and uid of zero if the slot has
       never been used or the slot has been used and the UI that was
       using exited cleanly. A random slot is picked and if that
       slot is already taken, a linear search is done to find the
       first open one. If it can't find an empty slot, it will check
       to see if the pids listed in the slots are still active (they
       not be active if a UI died) and chose the first available slot. */

    /* Randomly choose which slot to use for user memory to reduce the
       chance of two UIs grabbing the same slot if they are started at the
       same time. */

    /* Generate a seed */
    srandom(uid + pid);

    start_point = random() % USER_MAX;


    /* First, look for completely empty user memory slot */
    for (i = start_point; i < USER_MAX; i ++)
      {
	if (uiuser[i] == NULL)	/* User Shared Memory not created yet */
	  return(-1);
	if (uiuser[i]->p_id == 0 && uiuser[i]->user_id == 0)
	    return(i);
      }

    for (i = 0; i < start_point; i ++)
	if (uiuser[i]->p_id == 0 && uiuser[i]->user_id == 0)
	    return(i);


    /* Then, look for an empty user slot where the process no longer exists.
       A kill() with a signal of 0 will return a -1 and an errno of ESRCH if the process is no
       longer there. */
    for (i = start_point; i < USER_MAX; i ++)
	if (uiuser[i]->p_id != 0)
	  {
	    rc = kill(uiuser[i]->p_id, 0);
	    if (rc == -1 && errno == ESRCH)
	      return(i);
	  }

    for (i = 0; i < start_point; i ++)
	if (uiuser[i]->p_id != 0)
	  {
	    rc = kill(uiuser[i]->p_id, 0);
	    if (rc == -1 && errno == ESRCH)
	      return(i);
	  }

    /* User space must have been full */
    return(-1);
}/* end get_user_index()  */

