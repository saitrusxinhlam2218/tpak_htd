static char sz__FILE__[]="@(#)ui_init.c        10.1.2.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  ui_init.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:50:52
* @(#)  Last delta: 12/2/94 at 18:29:50
* @(#)  SCCS File: /taxi/sccs/s.ui_init.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <curses.h>
#include <signal.h>
#include <pwd.h>
#include <grp.h>
#include <setjmp.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <utmp.h>
#include <pwd.h>
#include <isam.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/sysinfo.h>
#ifdef __alpha
#include <sys/proc.h>
#endif
#include <langinfo.h>

#include "taxipak.h"
#include "Object.h"
#include "Fleet_public.h"
#include "Fleet_private.h"

#include "mads_types.h"
#include "fleet.h"
#include "mad_ipc.h"
#include "menu.h"
#include "user.h"
#include "calls.h"
#include "except.h"
#include "ui_error.h"                           /* errno number file for user interface */
#include "ui_def.h"                             /* contains definitions used by usesr interface */
#include "screens.h"                            /* contains names of screens that this program uses = saved in variable scr_name */
#include "key.h"
#include "mad_error.h"
#include "language.h"
#include "ui_strdefs.h"
#include "acc.h"
#include "veh_driv.h"
#include "enhance.h"
#include "switch_ext.h"
#include "Meter_action_db.h"
#include "Call_msg_public.h"
#include "Call_msg_private.h"
#include "Call_msg_db.h"
#include "Call_lists_public.h"
#include "Line_Manager_db.h"

#define         NUM_SEALS               16
extern  char    *getenv();
extern int No_exception_beep;
int last_total_count = 0;

#define GPS_TTY_FILE "/usr/taxi/etc/gps_ttys"
char    sGPS_host[64];

/* global variables */
char    sLatDescript[64];
char    sTelnetHost[64];
extern int iserrno;                             /* cisam errno */
short mem_attached = 0;                         /* 0=shared memory not attached; 1=it is attached; is used by cleanup() */
short line_mgr_attached = 0;
short curses_set=0;                             /* =1,if curses is initilzied,else 0; cleanup() looks at it */
int scr_name;                                   /* name of screen on display */
int fifo_fd;                                    /* file descriptor for opening FIFO */
int gps_pipe_fd;
int msgkey;                                     /* for storing msg queue id */
int shmid;                                      /* for saving shared memory segment id */
int semid;                                      /* for saving semaphore id */
char  *LineMgrshmptr;
int   LineMgrshmid;
struct fleets *fleet[FLEET_MAX];                /* array of pointers to point to fleet structures in shared memory */
struct offsets *offset;                         /* pointer to point to the offset structure in shared memory */
struct users *uiuser[USER_MAX];                 /* array of pointers to point to users structure in shared memory */
struct menus *cmenu[MENU_MAX];                   /* array of pointers to point to menus  structure in shared memory */
int uid;                                        /* users id as in /etc/passwd */
int user_index;                 /* index to be used by this user for the users structure in  structure in shared memory */
extern char req_buf[];                          /* buffer for reading/writing cisam records */
FILE *fd_other;                                 /* file descriptor for use with 2nd terminal */
char *A_shm_ptr;                                  /* pointer to beginning of shared memory segment, as returned  by shmat() */
extern int errno;                               /* system generated errno */
extern char *sys_errlist[];
int map_array[MAX_MENU_LIST];   /* array to convert scr_name into the index into the menus structure in shared memory */
extern char *ptr_item_desc[];   /* array of pointers to point to item desc saved in item_desc[] buffer, for display on menu */
extern char item_desc[];                        /* buffer for saving item desc for display on menus */
int pid;                                        /* process id of this user interface process */
extern char *ui_errlist[];                              /* array of pointers to ui error msgs */
extern int (*security[])();                     /* array of pointers to functions for checking users permissions */
FILE *trace_fp;                                 /* pointer to open trace file */
WINDOW *srch_win;                               /* pointer to searches window */
WINDOW *msg_win;
WINDOW *alt_win;
WINDOW *help_win;                               /* pointer to help window */
WINDOW *newwin();                               /* function for creating new windows */
char canmtxt[MAX_CANM+1][41];                   /* array to store canned mesg text */
char seal[NUM_SEALS][4];                        /* stores seal abbrevs for search screens */
extern int No_status_line;
int     basecall;
extern int def_fl_index;
extern int No_time_update;
extern int Dont_check_for_dispatch;
int Dispatch_pid;
extern char *Status_line_start;
extern char *Status_line_end;
int    ui_phone=0;
int    ui_phone_update=0;
int    ui_tlg;
int    ui_acdcli;
int    GPS_guy=0;
int GPS_allowed = 0;
extern sigjmp_buf   jmpbuf;
int    do_longjmp = 0;
extern int ok_to_jmp;
extern int emerg_in_progress;
extern long glEnhSwitch; 
int LastVehUsed = 0;
short StatusLineFleet = 0;
CALL_MSG_LIST_HNDL  cmsg_list_hndl;                     /* list structure for tracking messages attached to a call */

struct exception_count_struct *exception_count;
char exception_group_desc[EXC_GROUPS][8];

int     LineMgrIndex = 0;

void stophandler(int);

/* declare functions for checking fleet permissions */
extern int ui_fleet_perms0(), ui_fleet_perms1(), ui_fleet_perms2(), ui_fleet_perms3(), ui_fleet_perms4(), ui_fleet_perms5(), 
           ui_fleet_perms6(), ui_fleet_perms7();

/* declare array of pointers to functions for checking fleet perm's */
int (*ui_fleet_perms[])() = {ui_fleet_perms0,ui_fleet_perms1, ui_fleet_perms2, ui_fleet_perms3, ui_fleet_perms4, ui_fleet_perms5,
                             ui_fleet_perms6, ui_fleet_perms7};

#ifdef DB_PROFILE
#include "db_prof.h"

/* routine to attach to the shared memory used to track UI database activity.
 * The stats are logged by db() to a shared memory segment set up for ui
 * the same stats are kept for dispatch in a different memory segment.
 */
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

/* signal handler for signals that caurses UI to stop */
void
stophandler(sig)
int sig;
{
  char error_str[256];
  void cleanup();

  sprintf(error_str,"UI stopped by a signal = %d",sig);
  ERROR(' ',"ui_init", error_str);
  cleanup();
}

/* handler for fatal signals */
fatalhandler(sig)
int sig;
{
  char error_str[256];

  sprintf(error_str,"UI stopped by a fatal signal = %d",sig);
  ERROR(' ',"ui_init", error_str);
  signal(SIGILL,SIG_DFL);
  abort();
}

/* ============================================================================================================================ *
 * init: initializing routine for the user interface program. It opens various files requied by the program during run time.    *
 * the function also initializes various global data structure used during the life time of the program.
 *      Returns: DONE -         if initialization is completed successfully
 *               NOT_DONE -     if an error occurs in initialization
 * ============================================================================================================================ */

init()
{
        int addr;                       /* address where shared memory should be attached */
        int seg_size;                   /* size of shared memory segment to be attached */
        int i,j;                        /* scratch variable */
        struct cisam_up *user_ptr;      /* pointer to cisam_up structure for reading user profile record from cisam file on disk */
        extern void cleanup();          /* cleanup routine called before exiting */
        void sig_noop();                /* a NOOP routine to catch SIGINT and SIGQUIT */
        void display_time();
        int check_if_dispatch_is_up();
        char ch_nbr;                    /* for accessing the attributes file */
        char time_str[7];
        char term_name[32];
        struct cisam_at *attr_ptr;      /* to access attributes file */
        struct cisam_cm *cm_ptr;        /* to access canned mesg file */
        struct cisam_cl *cl_ptr;        /* to access call file */
        struct cisam_sl *sl_ptr;        /* pointer to seal file */
        struct cisam_eg *eg_ptr;
        int read_flag;                  /* Stores status for db() read */
        void cleanup();
        void emergency_notify();
        void GetClientInfo();        
        struct users *uiptr;
	int sysinfobuf[2];
	char *pLangInfo;
	char    bufTTY[132];
	char    sDefinition[64];
	FILE    *fpTTYforGPS, *fpLatDescript;
	char    tmpLatFileName[32];
	char    SysBuff[64];
	char    *sMyTTY;
	char    *pServer, *pPort, *pSpace;
	char    *pHostname, *pCR;
	struct utmp  *pUtmp;
	char    *user_name;
	struct passwd *pPwd;
        LINE_MGR_DB_REC  line_mgr_rec;
        LINE_MGR_IPC    MesgLineMgr;
        LINE_MGR_STRUCT  *pLineMgrShmem;
        LINE_MGR_DB_REC  *pLineMgrRec;
        TAO_QUEUE_STRUCT     *pCallQueueShmem;
        struct passwd *PasswordEntry;        
        char    *tmp_shmptr;
        int     mode, done;
        
	
        /* initialize signal catchers */

	/** Disable UAC Messages **/
#ifdef __alpha
	sysinfobuf[0] = SSIN_UACPROC;
	sysinfobuf[1] = UAC_NOPRINT;
	i = setsysinfo( SSI_NVPAIRS, sysinfobuf, 1, NULL, NULL, 0 );
#endif
	
        /* Initialize all signals to use the default signal handler */
	/**
        for (i=SIGQUIT; i<=SIGSYS; i++) signal(i,fatalhandler); **/
	//        signal(SIGSTOP,stophandler);
	//        signal(SIGTTIN,stophandler);
	//        signal(SIGTTOU,stophandler);
        /* If the environment variable UI_SIG is set, an interupt will
           shut down UI and it's possible to send a stop or a kill from
           the keyboard. Otherwise if UI_SIG is not set, those signals
           are ignored. */
        if (getenv("UI_SIG"))
          {
            signal(SIGINT, cleanup);
          }
        else
          {
            signal(SIGINT, sig_noop);
            signal(SIGQUIT, SIG_IGN);

            /* STOP from keyboard */
            signal(SIGTSTP, sig_noop);
          }
	if (getenv("UI_TLG"))
	  ui_tlg = 1;	
	if (getenv("UI_PHONE"))
	  ui_phone = 1;
        if (getenv("UI_PHONE_UPDATE"))
          ui_phone_update = 1;
	if (getenv("UI_GPS"))
	  GPS_guy = 1;	
        signal(SIGHUP, cleanup);
        signal(SIGPIPE, cleanup); /*occurs if writing to pipe with no dispatch*/       signal(SIGSEGV, cleanup);
	signal(SIGBUS, cleanup);
        signal(SIGTERM, cleanup);
        signal(SIGUSR1, cleanup);
        signal(SIGCONT, GetClientInfo);        
        if (!No_time_update)
            signal(SIGALRM, display_time);

        /* Sent by dispatch on emergencies */
        signal(SIGUSR2, emergency_notify);

	//	UI_List_services_start();
	
        /*
         * call routines to initialize all the string arrays required
         */

	
	pLangInfo = nl_langinfo( NOSTR );
        ui_errlist_init();
        ui_init_response_time_msgs();
        ui_syserr_init();

        if (!Dont_check_for_dispatch && check_if_dispatch_is_up() == -1)
          {
            prt_error(UI_NO_MSG, "%s\n%s",
                      catgets(UI_m_catd, UI_1, UI_TEXT_1, "Cannot start. Dispatch is either not running or is still coming up."),
                      catgets(UI_m_catd, UI_1, UI_TEXT_2, "Please try again in a moment ..."));
            return(NOT_DONE);
          }

#ifdef DEBUG
        /* open the trace file to be used by the user interface program */
        if ( (trace_fp = fopen(TRACE_FILE,"a")) == NULL ) {
                /* cannot open trace file */
                fprintf(stderr, "%s %s <%s>",TRACE_FILE,ui_errlist[UI_OPEN],sys_errlist[errno]);
                return(NOT_DONE);
        }  /* end if */

        /* Make trace file world readable and writable */
        chmod(TRACE_FILE, "00666");
#endif

	//#ifdef __alpha

	if ( (fpTTYforGPS = fopen(GPS_TTY_FILE, "r")) == NULL )
	  {
	    fprintf(stderr, "Unable to open GPS_TTY_FILE");
	  }
	else
	  {
	    sMyTTY = ttyname( 0 );
	    sMyTTY = &sMyTTY[5];
	    sprintf(tmpLatFileName, "/tmp/latstr.%d", getpid());
	  }

	if ( 1 )
	  {
	    uid = getuid();
	    pPwd = getpwuid( uid );

	    user_name = pPwd ? pPwd->pw_name : "root";
	    
	    setutent();
	    while ( ( pUtmp = getutent() ) != NULL )
	      {
		if ( pUtmp->ut_type == USER_PROCESS &&
		     (!strcmp( pUtmp->ut_user, user_name ) ) )
		  {
		    if ( ( strlen( pUtmp->ut_host ) ) &&
			 ( strcmp( pUtmp->ut_host, ":0.0" ) ) &&
			 ( !strcmp( pUtmp->ut_line, sMyTTY ) ) )
		      {
			strcpy( sTelnetHost, pUtmp->ut_host );
			break;
		      }
		  }
	      }
	    
	  }
		     

	if ( ( sMyTTY != NULL ) )
	  {
	    if ( !strlen( sTelnetHost ) )
	      {
		sprintf( SysBuff, "/usr/sbin/latcp -d -P -p%s > %s", sMyTTY, tmpLatFileName );
		system( SysBuff );
		
		fpLatDescript = fopen( tmpLatFileName, "r" );
		
		if ( fpLatDescript != NULL )
		  {
		    fgets( sLatDescript, 132, fpLatDescript ); /** Blank Line **/
		    if ( strncmp( sLatDescript, "No terminal", 11 ) )
		      {
			fgets( sLatDescript, 132, fpLatDescript ); /** Local Port Name = #####    <interactive> **/
			fgets( sLatDescript, 132, fpLatDescript ); /** Blank Line **/
			fgets( sLatDescript, 132, fpLatDescript ); /**         Actual Remote Node Name = ESERV1 **/
			pServer = strstr( sLatDescript, "Name = " );
			pServer = pServer + 7;
			pServer[ strlen( pServer ) - 1 ] = '\0';
			strcpy( sDefinition, pServer );
			fgets( sLatDescript, 132, fpLatDescript ); /**         Actual Remote Port Name = PORT_1 **/
			pPort = strstr( sLatDescript, "Name = " );
			pPort = pPort + 7;
			pPort[ strlen( pPort ) - 1 ] = '\0';
			strcat( sDefinition, ":" );
			strcat( sDefinition,  pPort );
			sDefinition[ strlen(sDefinition) ] = '\0';
			strcpy( sLatDescript, sDefinition );
		      }
		  }
	      }
	
	    if ( strlen( sTelnetHost ) )
	      strcpy( sLatDescript, sTelnetHost );
	    
	    if ( GPS_guy ) /* we're testing */
	      strcpy( sLatDescript, "ESERV1:PORT_4" );

            if (getenv("UI_PHONE_TEST"))
              strcpy( sLatDescript, "TSERV3:PORT_1" );
            
	    if (sLatDescript != NULL)
	      {
		bzero( sGPS_host, sizeof( sGPS_host ) );
		while (fgets(bufTTY, 64, fpTTYforGPS) != NULL)
		  {
		    if (bufTTY[0] != '#') /* Comment line */
		      {
			pSpace = (char *) strchr( bufTTY, '\t' );
			if ( pSpace != NULL )
			  *pSpace = '\0';
			if (!strcmp(bufTTY, sLatDescript))
			  {
			    *pSpace = '\t';
			    GPS_guy = TRUE;
			    pHostname = (char *)strchr(bufTTY, '\t');
			    ++pHostname;
			    pCR = (char *)strchr(pHostname, '\n');
			    *pCR = '\0';
			    strcpy( sGPS_host, pHostname );
			  }
		      }
		  }
	      }
	    fclose(fpTTYforGPS);
	    unlink( tmpLatFileName );

            // Find LINE_MGR record for this TTY or telnet host
            db_open( LINE_MGR_FILE_ID, ISINOUT | ISMANULOCK );
            done = 0;
            mode = ISFIRST;
            while ( ( db_read_key( LINE_MGR_FILE_ID, &line_mgr_rec, &line_mgr_key1, mode ) == SUCCESS ) && ( !done ) )
              {
                mode = ISNEXT;
                if ( !strcmp( line_mgr_rec.DDS_Server, sLatDescript ) )
                  {
                    LineMgrIndex = line_mgr_rec.DDS_index;
                    done = 1;
                  }
              }
            db_close( LINE_MGR_FILE_ID );

            if ( LineMgrIndex > 0 )
              {
                // attach to line_mgr shared memory
                line_mgr_attached = 0;
                seg_size = sizeof( struct LineMgr_offsets )
                  + MAX_TTYS * sizeof( LINE_MGR_STRUCT )
                  + QUEUE_SIZE * sizeof( TAO_QUEUE_STRUCT );
                if ( ( LineMgrshmid = shmget( LINE_MGR_SHMEM_KEY, seg_size, 0666 ) ) == -1 )
                  {

                  }
                else
                  {
                    if ( ( LineMgrshmptr = shmat( LineMgrshmid, 0, 0 ) ) == (char * ) -1 )
                      {

                      }
                    else
                      {
                        pLineMgr_offset = ( struct LineMgr_offsets * ) LineMgrshmptr;
                        tmp_shmptr = (char *) ++pLineMgr_offset;
                        pLineMgrShmem = (LINE_MGR_STRUCT *) tmp_shmptr;
                        for (i = 0; i < MAX_TTYS; i++)
                          Lines[ i ] = pLineMgrShmem++;
                        pCallQueueShmem = (TAO_QUEUE_STRUCT *)pLineMgrShmem;
                        for ( i = 0; i < QUEUE_SIZE; i++ )
                          {
                            TaoCallQueue[ i ] = pCallQueueShmem++;
                          }
                        line_mgr_attached = 1;
                        PasswordEntry = getpwuid(getuid());
                        strcpy( Lines[LineMgrIndex]->UserName, PasswordEntry->pw_name );
                        Lines[LineMgrIndex]->UserID = getuid();

                        for ( i = 0; i < MAX_PIDS; i++ )
                          {
                            if ( Lines[LineMgrIndex]->UIPid[i] == 0 )
                              {
                                Lines[LineMgrIndex]->UIPid[i] = getpid();
                                break;
                              }
                          }

                      }
                  }
              }
          }
//#endif
	/* open GPS pipe to gps_proc                       */
	if ((gps_pipe_fd = open(GPS_PIPE,O_RDWR | O_NDELAY)) < 0 )
	  fprintf(stderr,"Can't open GPS pipe\n");
	
        /* open FIFO to write requests to dispatch process */
        if((fifo_fd = open(USER_PIPE, O_RDWR | O_NDELAY)) == -1)  {
                /* cannot open FIFO */
                fprintf(stderr, "%s %s %s\n",ui_errlist[UI_OPEN], USER_PIPE,sys_errlist[errno]);
                return(NOT_DONE);       /* return to main() */
        }

        /* get id of the msg queue used to pass messages between the user interface and the dispatch program */
        if ( (msgkey = msgget(MSGKEY,IPC_CREAT|0666)) == -1 )  {
                /* msgget failed */
                fprintf(stderr,"%s %s\n",ui_errlist[UI_MSGGET],sys_errlist[errno]);
                return(NOT_DONE);       /* return to main() */
        }  /* end msgget */

        /* determine the size of the shared memory segment to be used */
        seg_size = sizeof(struct offsets) + FLEET_MAX*sizeof(struct fleets) + CALLS_MAX*sizeof(struct calls) + 
            USER_MAX*sizeof(struct users) + EXCEPT_MAX*sizeof(struct excepts) + MENU_MAX*sizeof(struct menus) +
            EXCEPT_DESC_MAX * sizeof(struct excpt_desc)
              + sizeof(struct exception_count_struct) + RES_PERIOD_MEM_SIZE + HOLIDAY_MEM_SIZE;

        /* get the id for the shared memory segment to be attached */
        if ( (shmid = shmget(SHMKEY,seg_size,0666)) == -1 )  {
                /* shmget failed */
                fprintf(stderr,"%s %s\n", ui_errlist[UI_SHMGET],sys_errlist[errno]);
                return(NOT_DONE);       /* back to main() */
        }  /* end shmget */

        /* attach the shared memory segment */
        if ( (A_shm_ptr = shmat(shmid, (const void *)0x0a00000010000000, 0)) == (char *)-1 )  {		
                /* shmat failed */
                fprintf(stderr,"%s %s\n", ui_errlist[UI_SHMAT],sys_errlist[errno]);
                return(NOT_DONE);       /* back to main() */
        }  /* end if shmat */

        /* shmat worked; set the mem_attached which cleanup looks at */
        mem_attached = 1;


        /* get semaphore id to be used to lock the array of excepts in  shared memory. Note that "Dispatch" should 
         * create the set of 2 semaphores - a read semaphore and a write semaphore
         */
        if((semid = semget(SEMKEY,NSEMS,0666|IPC_CREAT)) == -1)  {
                /* semget failed */
                fprintf(stderr,"%s %s\n", ui_errlist[UI_SEMID], sys_errlist[errno]);
                return(NOT_DONE);       /* return to main() */
        } 

        /* initialize pointers for accessing structures/variables in shared memory segment */

        /* initialize the pointer to point to the offsets structure in shared memory */
        offset  = (struct offsets *)A_shm_ptr;

        /* Exception count is at end of shared memory (need to make this less kludgy) */
        exception_count = (struct exception_count_struct *) offset->except_count;

        if (db(EXC_GROUP_ID, OPEN, &eg_key1, ISINOUT + ISMANULOCK, 0) < 0)
          {
            ERROR(' ',EXCGRP_FILE,"Error opening file\n");
            return(NOT_DONE);
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
            fprintf(stderr, "Error closing file %s\n", EXCGRP_FILE);
            return(NOT_DONE);
          }

        /* initialize pointers to fleet structure in shared memory */
        fleet[0] = offset->fleet;
        for (i = 1; i < FLEET_MAX; i++)
                fleet[i] = fleet[0] + i;
	LastVehUsed = offset->veh_used;
	
        /* initialize pointers to users structure in shared memory */
        uiptr = offset->user;
        uiuser[0] = uiptr++;
        for(i = 1; i < USER_MAX; i++ )
                uiuser[i] = uiptr++;

        /* initialize pointers to menu structure in shared memory */
        cmenu[0] = offset->menu;
        for (i = 1; i < MENU_MAX; i++ )
                cmenu[i] = cmenu[0] + i;
        /* initialize map_array[] so that the map_array[scr_name] gives the index into the menus structure in shared memory.
         * This indexing method helps in calling function thru pointers to functions, hence avoiding huge case statements
         */
        for(i=0;i<MAX_MENU_LIST;i++)
                map_array[i] = -1;      /* initialize, for checking by menu_manager() */

        map_array[MAIN_MENU] = 0;
        map_array[CALL_ENTRY_MENU] = 1;
        map_array[SEARCHES_MENU] = 2;
        map_array[SUPERVISORY_MENU] = 3;
        map_array[UPDATE_USER_DEFAULTS] = 4;
        map_array[SYSTEM_ACTIVITIES_MENU] = 5;
        map_array[USER_DEFINED_MENU] = 6;
        map_array[BASIC_CALL_SCR] = 7;
        map_array[EXTENDED_CALL_SCR] = 8;
        map_array[WAKEUP_CALL_SCR] = 9;
        map_array[SUBS_BASIC_SCR] = 10;
        map_array[SUBS_EXTENDED_SCR] = 11;
        map_array[SUBS_WAKEUP_SCR] = 12;
        map_array[CALL_USER_DEF1] = 13;
        map_array[CALL_USER_DEF2] = 14;
        map_array[CALL_USER_DEF3] = 15;
        map_array[UNASSIGNED_CALLS_SCR] = 16;
        map_array[SUBS_FUTURE_CLS_SCR] = 17;
        map_array[ASSIGNED_CALLS_SCR] = 18;
        map_array[ALL_CALLS_SCR] = 19;
        map_array[VEH_DR_CALLS_SCR] = 20;
        map_array[INDIVIDUAL_CALLS_SCR] = 21;
        map_array[EXCEPTIONS_SCR] = 22;
        map_array[OTHER_SEARCHES_MENU] = 23;
        map_array[SRCH_USER_DEF1] = 24;
        map_array[SUSPEND_VEH_DR_SCR] = 25;
        map_array[ZONE_STATUS_SCR] = 26;
        map_array[SYSTEM_STATUS_SCR] = 27;
        map_array[EDIT_REPEAT_MESSAGES] = 28;
        map_array[SUPERV_USER_DEF1] = 29;
        map_array[SHIFT_SCHEDULER] = 30;
        map_array[SUPERV_USER_DEF3] = 31;
        map_array[SUPERV_USER_DEF4] = 32;
        map_array[SUPERV_USER_DEF5] = 33;
        map_array[UM_USER_DEF_1] = 34;
        map_array[UM_USER_DEF_2] = 35;
        map_array[UM_USER_DEF_3] = 36;
        map_array[UM_USER_DEF_4] = 37;
        map_array[UM_USER_DEF_5] = 38;
        map_array[UM_USER_DEF_6] = 39;
        map_array[UM_USER_DEF_7] = 40;
        map_array[UM_USER_DEF_8] = 41;
        map_array[UM_USER_DEF_9] = 42;
        map_array[VEHICLE_SRCH_SCR] = 43;
        map_array[DRIVER_SRCH_SCR] = 44;
        map_array[USER_SRCH_SCR] = 45;
        map_array[OS_USER_DEF1] = 46;
        map_array[OS_USER_DEF2] = 47;
        map_array[OS_USER_DEF3] = 48;
        map_array[OS_USER_DEF4] = 49;
        map_array[OS_USER_DEF5] = 50;
        map_array[OS_USER_DEF6] = 51;

        /* initialize array of pointers ptr_item_desc[] to point to appropriate  position in item_desc[], 
         * for saving the item desc  as given by mf_item_desc buffer in menus structure */
        for ( i=0; i < MAX_MENU_ITEMS; i++ )
                ptr_item_desc[i] = &item_desc[i*41];

        /* get the process id # */
        pid = getpid();

        /* get the users id # */
        uid = getuid();

        if ((user_index = get_user_index()) == -1)
            return(NOT_DONE);

        /* Immediately fill user data for pid and uid so no
           other UI can grab it */
        uiuser[user_index]->p_id = pid;
        uiuser[user_index]->user_id = uid;

        user_ptr = (struct cisam_up *)req_buf;
        user_ptr->user_id = (short)uid;

/*  This is done by the ERROR macro in common/error.c:mad_error()
open the error file on disk
        if(db(ERROR_FILE_ID,OPEN,&el_key1,ISINOUT+ISMANULOCK,0) < 0) {
                fprintf(stderr,"%s %s %d\n", ui_errlist[UI_OPEN], catgets(UI_m_catd, UI_1, UI_UI_INIT_2, "ERROR_FILE"),iserrno);
                return(NOT_DONE);
        }
*/
        /* open the user's profile file on disk */
        if(db(USER_FILE_ID,OPEN,&up_key1,ISINOUT+ISMANULOCK,0) < 0) {
                /* db failed to read record from disk */
                fprintf(stderr,"%s %s %d\n", ui_errlist[UI_OPEN], catgets(UI_m_catd, UI_1, UI_UI_INIT_3, "USER_FILE"),iserrno);
                return(NOT_DONE);
        }

	if (db(MAILBOX_FILE_ID,OPEN,&mb_key1,ISINOUT+ISMANULOCK,0) < 0) {	/* open mailbox file */
	  ERROR(' ',MAILBOX_FILE,"Error opening file");			/* if problem, ERROR */	
	  return(NOT_DONE);
	}
	
        /* read this user's record from the user profile file */
        if(db(USER_FILE_ID,READ_KEYS,&up_key1,ISEQUAL,0) < 0) {
                /* db failed to read record from disk */
                fprintf(stderr,"%s %s %d\n", ui_errlist[UI_DSKREAD], catgets(UI_m_catd, UI_1, UI_UI_INIT_3, "USER_FILE"),iserrno);
                return(NOT_DONE);
        }

        /* got users record in req_buf[] */

        /* fill the appropriate portion of the shared memory segment with the users information from user profile */
        fill_user();

    /* get the count of messages for the user */
    uiuser[user_index]->cnt_msg = (short)message_count(uiuser[user_index]->user_id);

        if (uiuser[user_index]->default_screen >= MAX_MENU_LIST || map_array[uiuser[user_index]->default_screen] == -1) {
            prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_UI_INIT_26, "You have an invalid default screen in your profile."));
            prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_UI_INIT_22, "Talk to your administrator to correct the problem."));
            return(NOT_DONE);
        }

        if (uiuser[user_index]->default_search >= MAX_MENU_LIST || map_array[uiuser[user_index]->default_search] == -1) {
            prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_UI_INIT_27, "You have an invalid default search screen in your profile."));
            prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_UI_INIT_22, "Talk to your administrator to correct the problem."));
            return(NOT_DONE);
        }

        /* fill_user() will have found the default fleet index */
        if (def_fl_index == -1) {
            prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_UI_INIT_21, "You have an invalid default fleet in your profile."));
            prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_UI_INIT_22, "Talk to your administrator to correct the problem."));
            return(NOT_DONE);
        }

        if (fleet[def_fl_index]->active != YES) {
            prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_UI_INIT_23, "Your default fleet is not active."));
            prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_UI_INIT_22, "Talk to your administrator to correct the problem."));
            return(NOT_DONE);
        }
                 
        /* is user allowed to work with this fleet */
        if ((*ui_fleet_perms[def_fl_index])() == NOT_ALLOWED) {
            prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_UI_INIT_24, "You do not have permission to use your default fleet."));
            prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_UI_INIT_22, "Talk to your administrator to correct the problem."));
            return(NOT_DONE);
        }

        /* open the call file on disk */
        if(db(CALL_FILE_ID,OPEN,&cl_key1,ISINOUT+ISMANULOCK,0) < 0) {
                /* db failed to read record from disk */
                fprintf(stderr,"%s %s\n", ui_errlist[UI_OPEN], catgets(UI_m_catd, UI_1, UI_UI_INIT_4, "CALL_FILE"));
                return(NOT_DONE);
        } 

        cl_ptr = (struct cisam_cl *)req_buf;
        if(db(CALL_FILE_ID, READ_KEYS, &cl_key1, ISLAST, 0) < 0) 
                basecall = 0;
        else
                basecall = cl_ptr->nbr;
        
        /* open the call history file on disk */
        if(db(CALLH_FILE_ID,OPEN,&ch_key1,ISINOUT+ISMANULOCK,0) < 0) {
                /* db failed to open call history file */
                fprintf(stderr,"%s %s\n", ui_errlist[UI_OPEN], catgets(UI_m_catd, UI_1, UI_UI_INIT_5, "CALLH_FILE"));
                return(NOT_DONE);
        }

        /* open the address file on disk */
        if(db(ADDRESS_FILE_ID,OPEN,&ad_key1,ISINOUT+ISMANULOCK,0) < 0) {
                /* db failed to read record from disk */
                fprintf(stderr,"%s %s\n", ui_errlist[UI_OPEN], catgets(UI_m_catd, UI_1, UI_UI_INIT_6, "ADDRESS_FILE"));
                return(NOT_DONE);
        }

        /* open the subzone file on disk */
        if(db(SUBZONE_FILE_ID,OPEN,&sz_key1,ISINOUT+ISMANULOCK,0) < 0) {
                /* db failed to read record from disk */
                fprintf(stderr,"%s %s\n", ui_errlist[UI_OPEN], catgets(UI_m_catd, UI_1, UI_UI_INIT_7, "SUBZONE_FILE"));
                return(NOT_DONE);
        }

        /* open the street name file on disk */
        if(db(STRNAME_FILE_ID,OPEN,&sn_key1,ISINOUT+ISMANULOCK,0) < 0) {
                /* db failed to read record from disk */
                fprintf(stderr,"%s %s\n", ui_errlist[UI_OPEN], catgets(UI_m_catd, UI_1, UI_UI_INIT_8, "STRNAME_FILE"));
                return(NOT_DONE);
        }

        /* open the street type file on disk */
        if(db(STRTYPE_FILE_ID,OPEN,&st_key1,ISINOUT+ISMANULOCK,0) < 0) {
                /* db failed to open street type file on disk */
                fprintf(stderr,"%s %s\n", ui_errlist[UI_OPEN], catgets(UI_m_catd, UI_1, UI_UI_INIT_9, "STRTYPE_FILE"));
                return(NOT_DONE);
        }

        /* open the locality file on disk */
        if(db(LOCALTY_FILE_ID,OPEN,&lo_key1,ISINOUT+ISMANULOCK,0) < 0) {
                /* db failed to open locality file on disk */
                fprintf(stderr,"%s %s\n", ui_errlist[UI_OPEN], catgets(UI_m_catd, UI_1, UI_UI_INIT_10, "LOCALITY_FILE"));
                return(NOT_DONE);
        }

        /* open the place name file on disk */
        if(db(PLACENM_FILE_ID,OPEN,&pl_key1,ISINOUT+ISMANULOCK,0) < 0) {
                /* db failed to open place name file on disk */
                fprintf(stderr,"%s %s\n", ui_errlist[UI_OPEN], catgets(UI_m_catd, UI_1, UI_UI_INIT_11, "PLACEN FILE"));
                return(NOT_DONE);
        }

        /* open the intersection file on disk */
        if(db(INT_FILE_ID,OPEN,&in_key1,ISINOUT+ISMANULOCK,0) < 0) {
                /* db failed to open place name file on disk */
                fprintf(stderr,"%s %s\n", ui_errlist[UI_OPEN], catgets(UI_m_catd, UI_1, UI_UI_INIT_12, "INTERS FILE"));
                return(NOT_DONE);
        }

        /* open the zone file on disk */
        if(db(ZONE_FILE_ID,OPEN,&zn_key1,ISINOUT+ISMANULOCK,0) < 0) {
                /* db failed to open zone file on disk */
                fprintf(stderr,"%s %s\n", ui_errlist[UI_OPEN], catgets(UI_m_catd, UI_1, UI_UI_INIT_13, "ZONE FILE"));
                return(NOT_DONE);
        } 

        /* open the vehicle file on disk */
        if(db(VEHICLE_FILE_ID,OPEN,&veh_key1,ISINOUT+ISMANULOCK,0) < 0) {
                /* db failed to open vehicle file on disk */
                fprintf(stderr,"%s %s\n", ui_errlist[UI_OPEN], catgets(UI_m_catd, UI_1, UI_UI_INIT_14, "VEHICLE FILE"));
                return(NOT_DONE);
        } 

        /* open the driver file on disk */
        if(db(DRIVER_FILE_ID,OPEN,&dr_key1,ISINOUT+ISMANULOCK,0) < 0) {
                /* db failed to open driver file on disk */
                fprintf(stderr,"%s %s\n", ui_errlist[UI_OPEN], catgets(UI_m_catd, UI_1, UI_UI_INIT_15, "DRIVER FILE"));
                return(NOT_DONE);
        }

        /* open the statusr file on disk */
        if(db(STATUSR_FILE_ID,OPEN,&su_key1,ISINOUT+ISMANULOCK,0) < 0) {
                /* db failed to open driver file on disk */
                fprintf(stderr,"%s %s\n", ui_errlist[UI_OPEN], catgets(UI_m_catd, UI_1, UI_UI_INIT_16, "STATUSR FILE"));
                return(NOT_DONE);
        }

        /* open the rate file */
        if ( db(RATE_FILE_ID,OPEN,&rt_key1,ISINOUT+ISMANULOCK,0) < 0 ) {
                fprintf(stderr,"%s %s\n", ui_errlist[UI_OPEN], catgets(UI_m_catd, UI_1, UI_UI_MISC_7, "<rate file>"));
                return;
        }

	if ( db(SUBCALL_FILE_ID, OPEN, &sc_key1, ISINOUT+ISMANULOCK,0) < 0 ){
		return;
        }

	if ( db(SUBCALLH_FILE_ID, OPEN, &subch_key1, ISINOUT+ISMANULOCK, 0) < 0 )
	  {
	  }

        db_open(GEOADDR_ADDR_FILE_ID, ISINOUT+ISMANULOCK);
        db_open(GEOADDR_LINE_FILE_ID, ISINOUT+ISMANULOCK);
        db_open(GEOADDR_POINT_FILE_ID, ISINOUT+ISMANULOCK);
	  
        cm_ptr = (struct cisam_cm *)req_buf;
        /* set up canned message text array for messages */
        if(db(CANMSG_FILE_ID,OPEN,&cm_key1,ISINOUT+ISMANULOCK,0) < 0) {
                /* db failed to open file */
                fprintf(stderr,"%s %s\n", ui_errlist[UI_OPEN], catgets(UI_m_catd, UI_1, UI_UI_INIT_17, "CANMSG_FILE"));
                return(NOT_DONE);
        }

        /* read records and store text in array */
        if (db(CANMSG_FILE_ID,READ_KEYS,&cm_key1,ISFIRST,0) >= 0) {
                for(i=0; i==0; ) {
                        if (cm_ptr->nbr > MAX_CANM)
                                fprintf(stderr,
                                        "%s <%d>\n",
                                        catgets(UI_m_catd, UI_1, UI_UI_INIT_18, "Canned mesg # out of range"), cm_ptr->nbr);
                        else
                                strncpy(&canmtxt[cm_ptr->nbr][0],cm_ptr->text,40);
                        if (db(CANMSG_FILE_ID,READ_KEYS,&cm_key1,ISNEXT,0) < 0)
                                i++;
                }
        }

        if (db(CANMSG_FILE_ID,CLOSE,&cm_key1,0,0) < 0) {
                /* db failed to close file */
                fprintf(stderr,"%s %s\n", ui_errlist[UI_CLOSE], catgets(UI_m_catd, UI_1, UI_UI_INIT_17, "CANMSG_FILE"));
                return(NOT_DONE);
        }

	if (db(CUSTOMER_FILE_ID, OPEN, &cust_key1, ISINOUT + ISMANULOCK ) )
	  {
	  }

	if (db(TIME_CALL_FILE_ID, OPEN, &time_call_key1, ISINOUT + ISMANULOCK ) )
	  {
	  }
	if (db(ACAD_FILE_ID, OPEN, &ACAD_key, ISINOUT + ISMANULOCK ) )
	  {
	  }
	if (db(INDV_FILE_ID, OPEN, &INDV_key, ISINOUT + ISMANULOCK ) )
	  {
	  }
	if (db(CALLMSG_FILE_ID, OPEN, NULL, ISINOUT+ISMANULOCK, NULL) < 0)
	  {
	  }
	
        /* Read in the seal abbrevs and store for search screens */

        sl_ptr = (struct cisam_sl *)req_buf;

        if(db(SEAL_FILE_ID, OPEN, &sl_key1, ISINOUT+ISMANULOCK, 0) < 0) {
                fprintf(stderr, "%s %s\n", ui_errlist[UI_OPEN], catgets(UI_m_catd, UI_1, UI_UI_INIT_19, "SEAL_FILE"));
                return(NOT_DONE);
        }

        read_flag = ISFIRST;

        for(i = 0; i < NUM_SEALS; i++) {
                if(db_read_key(SEAL_FILE_ID, sl_ptr, &sl_key1, read_flag) != SUCCESS) {
                        if(db(SEAL_FILE_ID, CLOSE, &sl_key1, 0, 0) < 0) 
                                fprintf(stderr, "%s %s\n", ui_errlist[UI_CLOSE], catgets(UI_m_catd, UI_1, UI_UI_INIT_19, "SEAL_FILE"));

                        break;
                }

                strcpy(seal[i], sl_ptr->abbrev);

                read_flag = ISNEXT;
        }

        db(SEAL_FILE_ID, CLOSE, &sl_key1, 0, 0);

       
        strcpy(term_name, getenv("TERM"));

        term_setup(term_name);

        /* configure the numeric keypad, send escape sequences to the terminal to set it into 24 lines & 132 col mode */
        config_tty();

        /* Check to see if a status line can be used */
        if (Status_line_start == NULL || Status_line_end == NULL)
          No_status_line = 1;

        /* initialize curses for the primary screen */

        initscr(); 

        noecho();

        /* create the search window */
        srch_win = newwin(SRCH_WIN_LINES, COLS, START_SRCHWIN_ROW, 0);

	msg_win = newwin(MSG_WIN_LINES, COLS - 2, START_MSG_WIN_ROW, 2);

	if (0)
	  alt_win = newwin(ALT_WIN_LINES, COLS - 5, ALT_WIN_START_ROW, 4 );

        cbreak();

        /* create the help window */
        help_win = newwin(SRCH_WIN_LINES,COLS,START_SRCHWIN_ROW,0);
        /* initialize keypad for the window */
        /*keypad(help_win,TRUE);*/

        /* since curses is initialized, so set the curses_set flag, which  cleanup() looks at */
        curses_set = 1;


        set_response_stat_flag();

        /* Start every minute alarm for displaying time */
        display_time();

        /* if the user has permission to display his/her default screen, the display it, otherwise display the MAIN_MENU */
        scr_name = -1;  /* initialize */
        if ( (*security[map_array[(int)uiuser[user_index]->default_screen]])() == NOT_ALLOWED )  {
                /* display MAIN_MENU */
                if ( menu_manager(YES, MAIN_MENU) != DONE )  {
                        /* cannot display MAIN_MENU */
                        prt_error(UI_NOSCR,"%d", MAIN_MENU);
                        return(NOT_DONE);       /* back to main() */
                }  /* end if */
        }  /* end if security() */
        else {
                /* display user's default screen */
                if ( menu_manager(YES, (int)uiuser[user_index]->default_screen) != DONE ) {
                        /* cannot display default menu */
                        prt_error(UI_NOSCR,"%d",(int)uiuser[user_index]->default_screen);
                        return(NOT_DONE);
                } /* end if */
        }  /* end else */

        /* initialize statusr array of structures for user stats */
        init_statusr_recs();

        /* If somehow another UI grabbed the user memory slot that we
           did, exit this UI and tell the other UI to exit. */
        if (uiuser[user_index]->user_id != uid) {
            prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_UI_INIT_25, "ERROR: User memory conflict. Please restart."));

            /* Give the other UI an error message to display on its status line */
            strcpy(uiuser[user_index]->img_25, catgets(UI_m_catd, UI_1, UI_UI_INIT_25,
                                                       "ERROR: User memory conflict. Please restart."));

            /* Set the other UI's pid to 0 so to signify that the other UI's
               status line is changed so user is aware of problem. */
            uiuser[user_index]->p_id = 0;
            return(NOT_DONE);
        }

        
        return(DONE);

}  /* end init()  */

int
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
}


/********************************************************************/
/* sig_noop - this routine does nothing except provide a place to go*/
/*            for the SIGINT and SIGQUIT signals.                   */
/********************************************************************/
void sig_noop()
{
}

/* Get the UNIX time of the next midnight. This is done by finding
   the UNIX time of the previous midnight and adding 24 hours. It is
   done this way instead of by finding the next date because you don't
   have to have to check if the date is at the end of month or year.
   */
time_t
get_next_midnight_time(t)
     time_t  t;
{
  struct tm now_time_struct, last_midnight_struct;
  long last_midnight_seconds, next_midnight_seconds; /* UNIX time */

  /* Get time in year/month/day/hour/minute/second in a struct tm */
  memcpy(&now_time_struct, localtime(&t), sizeof(struct tm));

  /* Determine the UNIX time (seconds since Jan 1, 1970) for
     the previous midnight */
  last_midnight_struct = now_time_struct;
  last_midnight_struct.tm_sec = 0;
  last_midnight_struct.tm_min = 0;
  last_midnight_struct.tm_hour = 0;
  last_midnight_seconds = timelocal(&last_midnight_struct);

  /* Next midnight is 24 hours = 86400 seconds later */
  next_midnight_seconds = last_midnight_seconds + 86400;
  return(next_midnight_seconds);
}

void
display_time()
{
    time_t t, next_alarm;
    static long last_t = 0;
    int  i, j, k, count, group, super_zone_num, total_count;
    char str[15], time_str[7];
    int  exception_group_count[EXC_GROUPS];
    static char status[133];
    static long midnight_time = 0;
    FLEET_HNDL   fleet_hndl;
    FLEET_EXCPT_LIST_HNDL fleet_excpt_list;
    struct excepts *excpt_ptr;
    EXCPT_HNDL    excpt_hndl;
    unsigned int  mask;
    check_mail();

    t = time(0);

    /* Set the next alarm to display time at each minute mark */

    if (!No_time_update)
      {
        next_alarm = 60 - (t % 60);
	signal(SIGALRM, display_time);	
	alarm(next_alarm);
      }

    if (t >= midnight_time)
      {
        /* If past midnight, put up new date, call entry screens have top line reverse and underline.
           other screens just reverse */
        if (scr_name == BASIC_CALL_SCR || scr_name == EXTENDED_CALL_SCR || scr_name == WAKEUP_CALL_SCR
            || scr_name == SUBS_BASIC_SCR || scr_name == SUBS_EXTENDED_SCR || scr_name == SUBS_WAKEUP_SCR)
          attrset( A_REVERSE | A_UNDERLINE );
        else
          attrset( A_REVERSE );

        /* Only need to print the date after start up */
        if (midnight_time != 0)
          {
            prt_date();
            refresh();
          }
        attrset(0);
        midnight_time = get_next_midnight_time(t);
      }

    get_srch_time(t, time_str);

    /* determine which fleet summary to display */
    bzero(exception_group_count, EXC_GROUPS * sizeof(int));

    if ( (StatusLineFleet + 1) >= FLEET_MAX )
      StatusLineFleet = 0;
    else
      ++StatusLineFleet;
    
    //    StatusLineFleet = (StatusLineFleet + 1) >= FLEET_MAX ? 0 : ++StatusLineFleet;
    
    while ( ( (*ui_fleet_perms[StatusLineFleet])() == NOT_ALLOWED ) ||
	    ( fleet[StatusLineFleet]->active != YES ) )
      {
        if ( (StatusLineFleet + 1) >= FLEET_MAX )
          StatusLineFleet = 0;
        else
          ++StatusLineFleet;
        //        StatusLineFleet = (StatusLineFleet + 1) >= FLEET_MAX ? 0 : ++StatusLineFleet;	        
      }

    
    sprintf(status, "%s  %c", time_str, fleet[StatusLineFleet]->fleet_id );

    /* Zero out the exception group counter */
    bzero(exception_group_count, EXC_GROUPS * sizeof(int));
    total_count = 0;
    /* First go through all the exception types */
    for (i = 0; i < EXC_TYPES; i ++)
      {
        /* # of this type of exception */
        count = 0;
        
        /* Only count for user's valid super zones. */
        for (j = 0; j < MAX_EXC_ZONES; j ++)
          {
            /* Get the actual super zone # from its index by
               looking in the fleet structure. */
            super_zone_num = fleet[StatusLineFleet]->super_zone[j].nbr;
            
            /* Since we can't guarantee that a user's super zones match
               the super zones for the default fleet (they should and the
               software should make sure that they do, but it hasn't been
               coded yet), we must go through all of the user's super zones
               to see if we found that was matched. */
            
            /* But first check to see if they super zone indexes do match */
            if (uiuser[user_index]->zones_sel[j] == super_zone_num)
              count += exception_count->count[StatusLineFleet][i][j];
            
            /* Otherwise go through the list of super zones */
            else
              for (k = 0; k < MAX_EXC_ZONES; k ++)
                if (uiuser[user_index]->zones_sel[k] == super_zone_num)
                  count += exception_count->count[StatusLineFleet][i][j];
          }
        
        /* Count the exceptions that don't fall in a zone. They are
           in the last slot (MAX_EXC_ZONES) in the superzone axis. */
        count += exception_count->count[StatusLineFleet][i][MAX_EXC_ZONES];
        
        /* Count the exceptions that aren't in a fleet. They are in the
           last slot of the fleet axis of the array and the last slot
           of the superzone axis */
        count += exception_count->count[FLEET_MAX][i][MAX_EXC_ZONES];
        
        if (count > 0)
          {
            /* The group this exception belongs to */
            group = uiuser[user_index]->exc_grp_slct[i - 1];
            
            exception_group_count[group - 1] += count;
          }
	total_count += count;
      }
        
    /* Now generate the status line based on the group counts */
    for (i = 0; i < EXC_GROUPS; i ++)
      {
        /* Exception groups 8, 9, and 11 are not used. There
           wouldn't be any room for them on the status line
           if they were used. */
        if (i == 8 || i == 9 || i == 11)
          continue;
        
        /* Each exception must take up exactly 13 chars on status line */
        
        count = exception_group_count[i];
        if (count > 0)
          {
            if(count < 100)
              sprintf(str,"   %7s=%02d", exception_group_desc[i], count);
            else
              sprintf(str,"   %7s=++", exception_group_desc[i]);
            strcat(status, str);

          }
        else
          /* 13 spaces */
          strcat(status, "             ");
      }

    if ( total_count > last_total_count )
      {
	if ( !No_exception_beep )
	  beep();
      }
    
    last_total_count = total_count;
    
    status_line(status);

    strcpy(uiuser[user_index]->img_25, status);

    /* Mark when time/exceptions was last updated (No particular reason; It used to be for counter) */
    uiuser[user_index]->last_25_time = t;
    last_t = t;
}


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
        if (uiuser[i]->p_id == 0 && uiuser[i]->user_id == 0)
            return(i);

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
}

