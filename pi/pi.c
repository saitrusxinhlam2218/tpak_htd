#if ( ENGLISH )
static char szTP_LANG[]="@(#)LANG = English" ;
#elif ( DANISH  )
static char szTP_LANG[]="@(#)LANG = Danish" ;
#elif ( FINNISH )
static char szTP_LANG[]="@(#)LANG = Finnish" ;
#elif ( GERMAN )
static char szTP_LANG[]="@(#)LANG = German" ;
#elif ( FRENCH )
static char szTP_LANG[]="@(#)LANG = French" ;
#elif ( SWEDISH )
static char szTP_LANG[]="@(#)LANG = Swedish" ;
#endif
static char sz__FILE__[]= "@(#)pi.c 10.1.2.3(ver) Date Release 95/01/24 13:25:47";

 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  pi.c; Rel: 10.1.2.3; Get date: 4/19/95 at 08:45:20
 * @(#)  Last delta: 1/24/95 at 13:25:47
 * @(#)  SCCS File: /taxi/sccs/s.pi.c
 *                                                                           *
 *****************************************************************************/
/* -----------------------------------------------------------------

    Copyright 1994 - TaxiCom Copenhagen, Denmark
    
    System   : PLANET - LINK
    Filename : PI.C
    Purpose  : To take care of the communication between PLANET
               and TaxiPak.

    Modification history

    Author      Date      Revision     Description
    ---------   -------   -----------  -------------------------
    CBT         940301    0.0          First try to make pi work

   ----------------------------------------------------------------- */ 

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <signal.h>
#include <setjmp.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/resource.h>
#include <sys/wait.h>

#define PI_ERROR( e, ExitValue ) mad_error( NULL, NULL, e,  __FILE__ , __LINE__ )

#include "language.h"
#include "except.h"
#include "pi.h"
#include "pi_struc.h"
#include "pi_inc.h"
#include "pi_var.h"
#include "enhance.h"
#include "switch_ext.h"
#include "taxipak.h"
#include "Object.h"
#include "mad_ipc.h"
#include "mads_isam.h"
#include "key.h"
#include "Line_Manager_private.h"
#define DECLARING
#include "ui_strdefs.h"
#include "comm_strdefs.h"
#include "writer_strdefs.h"
#include "Fleet_public.h"
#include "Fleet_private.h"

/* Globals */
struct socket_inf ClientSock;
int connections=0;
int using_monitor=1;
char  szMsgLog[200];
int HbSocket[2];
fd_set  ReadMask;
int msgkey;                                     /* for storing msg queue id */
int main_msgkey;
long glEnhSwitch; /* Enhancement switch for enhancement checking */
extern struct offsets *offset;
int      LineMgrshmid;
char     *LineMgrshmptr;
BOOLEAN  callback_active = FALSE;
BOOLEAN  callout_active = FALSE;
int      linemgr_cback_fd = -1;
int      linemgr_cout_fd = -1;

static sigjmp_buf  xts_jmpbuf;
int death_process();
int LostConnect();
void sig_noop();
MEM_ZONE *fleetMemZones[FLEET_MAX][ZONE_MAX];
void Zone_mem_init();

#define TRACE_MSG_FILE "/usr/taxi/run/traces/pi_log"

void	MakeLogEntry( char *szMsg )
{
   FILE *trace_logfile;
   CHAR *ctime(), *stime;
   time_t tmtime;
  struct tm *tm_ptr;   
   char    trace_file[65];
   char           DateBuff[16];

        tmtime = time((long *) 0);
        tm_ptr = localtime(&tmtime);
        
        sprintf(trace_file, "%s.%02d%02d%02d", TRACE_MSG_FILE,
                tm_ptr->tm_year % 100, tm_ptr->tm_mon + 1, tm_ptr->tm_mday);
        
	if ((trace_logfile = fopen(trace_file, "a")) == NULL)
          fprintf(stderr, "Error opening trace file \n");


        get_arg_date(tmtime, DateBuff);

        stime = ctime(&tmtime);
        fprintf(trace_logfile, "%.2s", DateBuff);
        fprintf(trace_logfile, "%.2s", &DateBuff[3]);
        fprintf(trace_logfile, "%.2s ", &DateBuff[6]);
        fprintf(trace_logfile, "%.2s", &stime[11]);
        fprintf(trace_logfile, "%.2s", &stime[14]);
        fprintf(trace_logfile, "%.2s", &stime[17]);        
        
        fprintf(trace_logfile, " %s\n", szMsg);

        fclose(trace_logfile);

}

 /**************************************************************************
  * Name: int init_con( struct socket_inf *Client)                         *
  * Type: Socket Info													   *
  *                                                                        * 
  * Purpose: To initialize the connection (port).                          *
  *                                                                        *
  * Returnvalue: 1 -> Done.                                                *
  *              0 -> Not Done.                                            *
  *                                                                        *
  * con: The connnection from where to find the pacnum.                    *
  **************************************************************************/

  int init_con( struct socket_inf  *Client)
  {
    Client->fd = NULL;
    Client->illegal_block = 0;
    Client->next_in_data_pos = 0;
    Client->next_out_data_pos = 0;
    Client->scan_state = PI_SCAN_WF_HEAD;
    Client->last_res_pacnum = -1;
    Client->this_pacnum = -1; 
    Client->last_out_pacnum = -1;
  } /* end init_con */


 /**************************************************************************
  * Name: int init_pi()                                                    *
  * Type: Function                                                         *
  *                                                                        * 
  * Purpose: To initialize the PI( Monitor notification).				   *
  *                                                                        *
  * Returnvalue: 1 -> Done.                                                *
  *              0 -> Not Done.                                            *
  **************************************************************************/

  int init_pi()
  {
    int openresult;
    int retry = 5,
        succeeded = 0;

    while (( retry > 0 ) && ( !succeeded ))
      if ( init_disp() == -1 )
        {
          fprintf(stderr, "PI-error: Not able to connect to TaxiPak - will try again in 15 secs.\n" );
          retry--;
          sleep( 15 );
        }
      else 
        succeeded = 1;
        
    if (( retry == 0 ) && ( !succeeded ))
      {
        fprintf(stderr, "PI-error: Not able to connect to TaxiPak within limit - will die\n" );
        exit(TAXI_ERROR);
      }



    return 1;
  } /* end init_pi */


 /**************************************************************************
  * Name: int set_options(int argc, char **argv)                           *
  * Type: Function                                                         *
  *                                                                        * 
  * Purpose: To initialize the pi with the parameters.                     *
  *                                                                        *
  * Returnvalue: 1 -> Done.                                                *
  *              0 -> Not Done.                                            *
  **************************************************************************/

  int set_options(int argc, char **argv)
  {
    int c;

    while ((c = getopt(argc, argv, "hms")) != EOF)
      switch(c)
      {
		case 'h':
			using_monitor = 0;
			break;
		
		case 's':
			bSilent = bTRUE;
			break;

      } /* end switch */
  } /* end set_options */


 /**************************************************************************
  * Name: int signal_to_monitor()                                          *
  * Type: Function                                                         *
  *                                                                        * 
  * Purpose: To tell monitor that PI still is running.                     *
  *                                                                        *
  * Returnvalue: 1 -> Done.                                                *
  *              0 -> Not Done (errors).                                   *
  **************************************************************************/

  int signal_to_monitor()
  {
    //    offset->status.poll_pi = 1;
  } /* end signal_to_monitor */


 /**************************************************************************
  * Name: int c_write( unsigned char *buf, int buf_len)            *
  * Type: Function                                                         *
  *                                                                        * 
  * Purpose: To send buf_len bytes from buf out on socket s.               *
  *                                                                        *
  * Returnvalue: -1    -> Illegal socket. Socket is not open.              *
  *              0     -> Nothing send at all.                             *
  *              n > 0 -> n bytes have been send out on socket s.          *
  *                                                                        *
  * con: The connection to send data on.                                   *
  * buf: The buffer where the data to send is stored.                      *
  * buf_len: The amount of bytes to send from buf.                         *                   
  **************************************************************************/

  int c_write( unsigned char *buf, int buf_len)
  {
    fd_set writeset;
    struct timeval select_timeout;
    int data_send = 0;
    unsigned char temp_string[256];
	int	nRet=0;

    if (ClientSock.fd == NULL)
    {
      init_con( &ClientSock);
      init_action();
      return 0;
    }

    FD_ZERO(&writeset);
    FD_SET( ClientSock.fd, &writeset);
    select_timeout.tv_sec = 7; /* timeout on writing is 7 seconds */
    select_timeout.tv_usec = 0;

	switch (select(FD_SETSIZE, NULL, &writeset, NULL, &select_timeout))
 	{
		
		case FAIL:
			sprintf( szLogMsg, "PI-error: Unable to write to socket <con:%d>",
													ClientSock.fd );
			MakeLogEntry( szLogMsg );
			exit(CLIENT_SOCK_WRITE);

		case (0):
			sprintf( szLogMsg, 
					"PI-error: Timeout on a write to a socket<con:%d>", 
														ClientSock.fd );
			MakeLogEntry( szLogMsg );
       		return data_send;

       	default:

          nRet = write(ClientSock.fd, buf, buf_len);


			if ( nRet != buf_len )
			{
				sprintf( szLogMsg, 
				"PI-error: Unable to write full message to socket <con:%d, msg sz %d>",
													ClientSock.fd, buf_len );
				MakeLogEntry( szLogMsg );
				exit(CLIENT_SOCK_WRITE);
			}

      } /* end switch */

    return data_send;

  } /* end c_write */


 /**************************************************************************
  * Name: int handle_new_socket(int nListenSock, fd_set *ReadMask) 		   *
  * Type: Function                                                         *
  *                                                                        * 
  * Purpose: Create a new PI server to service motor ( interface request ) *
  *                                                                        *
  * Returnvalue: 0 -> Not done.                                            *
  *              1 -> Done.                                                *
  *                                                                        *
  * s: The socket from where to read.                                      *
  **************************************************************************/

	int handle_new_socket(int nListenSock, fd_set *RdMask )
  {
    int length, length2,accept_fd, temp, connection_flags;
    char sIPAddr[64];
    struct sockaddr incomming;
    int nRet, i=0;
    struct sockaddr_in SockAddr;
    struct in_addr InAddr;
    int secs;
    int remoteAddr;
  struct sigaction action;
  sigset_t sig_set;
  int ret_setmask=0;    

  	length = sizeof(incomming);
	length2 = sizeof(SockAddr);

	//	sigsetjmp(xts_jmpbuf, 1);
	signal(SIGCHLD, death_process);
	
    accept_fd = accept( nListenSock, (struct sockaddr *) &incomming, &length);
    remoteAddr = getpeername( accept_fd, &SockAddr, &length2 );
    
    sprintf( sIPAddr, "%s", inet_ntoa( SockAddr.sin_addr ) );


	if ( accept_fd == FAIL )
	{
		perror("PI-ERROR (accept): ");
		exit(1);
		return FAIL;
	}

	sprintf( szLogMsg, "SuperServer: Accepted client on fd( %d)\n", accept_fd );
	MakeLogEntry( szLogMsg );
	
        nRet = 0;

	//#ifdef DAEMON
	sigemptyset(&sig_set);
	sigaddset(&sig_set, SIGCHLD);
	ret_setmask=sigprocmask(SIG_SETMASK, &sig_set, NULL);
	
	nRet = fork();

	sigemptyset(&sig_set);
	ret_setmask=sigprocmask(SIG_SETMASK, &sig_set, NULL);
	
	//#endif

	if ( nRet == -1)	/* Error forking */
	{
		perror( "PI-ERROR (fork)");
		return FAIL;
	}

	if ( nRet == 0 )  /* child */
	{
	  sprintf( szLogMsg, "Child process forked PID(%d) - IP(%s)", getpid(), sIPAddr );
	  MakeLogEntry( szLogMsg );

	  nRet = init_disp();
	  sprintf( szLogMsg, "init_disp returned %d", nRet);
	  MakeLogEntry( szLogMsg );
	  
          // open the pipes used for callout/callback
          if ((linemgr_cback_fd = open(CBACK_PIPE, O_RDWR | O_NDELAY)) == -1)
            {
              printf("Error opening CBACK_PIPE\n");
              exit(-1);
            }
          if ((linemgr_cout_fd = open(COUT_PIPE, O_RDWR | O_NDELAY)) == -1)
            {
              printf("Error opening COUT_PIPE\n");
              exit(-1);
            }
	  
	  FD_ZERO( &ReadMask );
	  close( ListenSock );

	  action.sa_handler = SIG_IGN;
	  action.sa_flags = SA_NOCLDWAIT;
	  sigaction(SIGCHLD, &action, NULL);
	  
	  signal(SIGCHLD, SIG_IGN);
	  signal(SIGURG, LostConnect);
	  signal(SIGPIPE, LostConnect);
	  ClientSock.fd = accept_fd;

	  bSuperServer = bFALSE;

	  ListenSock = accept_fd;
	  secs = 1;
	  if (setsockopt(ListenSock, SOL_SOCKET, SO_KEEPALIVE, &secs, sizeof(secs)) < 0)
	    printf("**TCP** -- setsockopt failed");
	  secs = 20;
	  if (setsockopt(ListenSock, IPPROTO_TCP, TCP_KEEPIDLE, &secs, sizeof(secs)) < 0)
	    printf("**TCP** -- setsockopt failed");
	  secs = 1;
	  if (setsockopt(ListenSock, IPPROTO_TCP, TCP_KEEPCNT, &secs, sizeof(secs)) < 0)
	    printf("**TCP** -- setsockopt failed");
	  secs = 20;
	  if (setsockopt(ListenSock, IPPROTO_TCP, TCP_KEEPINTVL, &secs, sizeof(secs)) < 0)
	    printf("**TCP** -- setsockopt failed");
	  
	  MakeLogEntry( "Server: Started" );
	}
	else			/* This is parent */
	{
	  sprintf( szLogMsg, "New Server proc #%d, pid (%d)", ++nChildren, nRet );
	  MakeLogEntry( szLogMsg );

	  close( accept_fd );

	}

  } /* end handle_new_socket */


 /**************************************************************************
  * Name: DumpDeadChildStat ( int Stat, int nProc, int nPid )  	           *
  * Class: Process util                                                    *
  *                                                                        * 
  * Purpose: Display post-mortem information on PI .                       *
  *                                                                        *
  *                                                                        *
  **************************************************************************/
void	DumpDeadChildStat( int nStat, int nFd, int nPid )
{

	sprintf( szLogMsg, "Process (pid=%d) on File Descriptor(%d) is dead\n", 
											nPid, nFd  );
	MakeLogEntry( szLogMsg );


	if ( WIFEXITED( nStat ) )
	{
		switch ( WEXITSTATUS( nStat ) )
		{
			case CLIENT_TERMINATES:
				sprintf( szLogMsg, "Termination, exit status %d(Client Terminates)\n", 
													WEXITSTATUS(nStat) );
				break;
	
			case TAXI_ERROR:
				sprintf( szLogMsg, "Termination, exit status %d(Accessing Taxi Funcs)\n", 
													WEXITSTATUS(nStat) );
				break;

			case RESOURCE_PROBLEM:
				sprintf( szLogMsg, "Termination, exit status %d(System Resources)\n", 
													WEXITSTATUS(nStat) );
				break;

			case CLIENT_SOCK_WRITE:
				sprintf( szLogMsg, "Termination, exit status %d(Error writing to client socket)\n", 
													WEXITSTATUS(nStat) );
				break;

			default:
				sprintf( szLogMsg, "Termination, exit status %d\n", 
													WEXITSTATUS(nStat) );
				break;
		}/* END SWITCH */

	}
	else if ( WIFSIGNALED( nStat ) )
		sprintf( szLogMsg, "Abnormal Termination, signal number = %d\n", 
							WTERMSIG(nStat) );
	else if ( WIFSTOPPED( nStat ) )
		sprintf( szLogMsg, "Child stopped, signal number = %d\n", 
												WSTOPSIG( nStat ) );
	else
		sprintf( szLogMsg, "Child stopped, no info\n" );

	
	MakeLogEntry( szLogMsg );
	// MakeSysExcept( 0, API_CHILD_DEATH );


}


 /**************************************************************************
  * Name: InitChildInfo ( )  	 								           *
  * Class: Process util                                                    *
  *                                                                        * 
  * Purpose: Intialize child-serer info arrays .	                       *
  *                                                                        *
  *                                                                        *
  **************************************************************************/
void	InitChildInfo( void )
{
	int	i;

	for ( i=0; i < MAX_CHILD; i++ )
	{
		ServChildInfo[i].nPid = NO_PI_CHILD;
		ServChildInfo[i].nFd = 0;
		ServChildInfo[i].nHeartBeatPipe = 0;
	}
}

 /**************************************************************************
  * Name: CheckChildren (int nCode ) 							           *
  * Class: Process util                                                    *
  *                                                                        * 
  * Purpose: Prompt kernel for info on all known children.	               *
  *                                                                        *
  *                                                                        *
  **************************************************************************/
int		CheckChildren( int nCode )
{
	int	StatLoc;
	int	nRet, i;
	int     rc = 0;

#ifdef FOO
	for ( i=0; i < nChildren; i++ )
	{
		/* Child already dead */
		if ( ServChildInfo[i].nPid  == NO_PI_CHILD  )
			continue;
		rc = kill( ServChildInfo[i].nPid, 0 );

		if ( rc == -1 && errno == ESRCH )
		  {
		    nRet = waitpid( ServChildInfo[i].nPid, &StatLoc, WNOHANG );
		    
		    if ( nRet == ServChildInfo[i].nPid )	/* Child has died */
		      {
			DumpDeadChildStat( StatLoc, ServChildInfo[i].nFd, ServChildInfo[i].nPid);
			ServChildInfo[i].nPid = NO_PI_CHILD;
			FD_CLR(ServChildInfo[i].nHeartBeatPipe, &ReadMask );
			close( ServChildInfo[i].nHeartBeatPipe);
			ServChildInfo[i].nHeartBeatPipe = 0;
			--nChildren;
		      }
		  }
	}
#endif
}
 /**************************************************************************
  * Name: int ReadClientData( struct socket_inf *Client)                   *
  * Type: Function                                                         *
  *                                                                        * 
  * Purpose: Read socket and return message ( if avail ) 				   *    
  *                                                                        *
  * Returnvalue: 0 -> Nothing to read and connection closed.               *
  *              1 -> Done.                                                *
  *              2 -> Done. ORDER-block ready with right length.           *
  *              3 -> Done. ORDER-block found but with wrong length.       * 
  *                                                                        *;
  * con: The connnectiopn from where to read.                              *
  **************************************************************************/

  int ReadClientData( struct socket_inf *Client) 
  {
    unsigned char temp_char; 
    int read_result,i;
    unsigned char temp_string[256];
    int cur_call_number, position;

    for(i=0; i<65536; i++) /* Read as many as possible instead of one */
    {
      //      if (Client->scan_state == PI_SCAN_WF_DATA)
      //        {
      //          read_result = read(Client->fd, Client->in_data[Client->next_in_data_pos], Client->in_data_len - 100);
      //        }
      //      else
	read_result = read(Client->fd, &temp_char, 1);

	if ( read_result == 0 )		/*Client has closed socket - end */
	{
	  return -1;
	  //		close( Client->fd );
	  //		exit( -1 );
	}

	if ( read_result == FAIL )
	{
          //		perror( "Error reading on fd (%d)\n" );
		return 0;
	}
#ifdef KEVIN
	sprintf( szLogMsg , "INPUT <%d> : offset (%d)\n",
											temp_char, Client->next_in_data_pos );
	MakeLogEntry( szLogMsg );
#endif
	switch (Client->scan_state)
	{
        case (PI_SCAN_WF_HEAD) :
          if (temp_char == HEAD_CHAR)
          {
            Client->in_data[0] = HEAD_CHAR;
            Client->next_in_data_pos = 1;
            Client->illegal_block = 0;
            Client->scan_state = PI_SCAN_WF_LEN1;
          }
	  break;

        case (PI_SCAN_WF_LEN1) :
          Client->in_data[Client->next_in_data_pos] = temp_char;
          Client->next_in_data_pos++;
          Client->in_data_len = temp_char;
          Client->scan_state = PI_SCAN_WF_LEN2;
	  break;

        case (PI_SCAN_WF_LEN2) :
          Client->in_data[Client->next_in_data_pos] = temp_char;
          Client->next_in_data_pos++;
          Client->in_data_len = Client->in_data_len 
												+ (temp_char*256);
          Client->scan_state = PI_SCAN_WF_PACNUM;
	  break;

        case (PI_SCAN_WF_PACNUM) :
          Client->in_data[Client->next_in_data_pos] = temp_char;
          Client->next_in_data_pos++;
          Client->last_res_pacnum = Client->this_pacnum;
          if (Client->last_res_pacnum == temp_char)
            Client->illegal_block = 1; 
          Client->this_pacnum = temp_char;
          Client->scan_state = PI_SCAN_WF_MSG1;
	  break;

        case (PI_SCAN_WF_MSG1) :
          Client->in_data[Client->next_in_data_pos] = temp_char;
          Client->next_in_data_pos++;
          Client->block_type = temp_char;
          Client->scan_state = PI_SCAN_WF_MSG2;
	  break;

        case (PI_SCAN_WF_MSG2) :
          Client->in_data[Client->next_in_data_pos] = temp_char;
          Client->next_in_data_pos++;
          Client->order_num = temp_char;
          if ((Client->in_data_len-(Client->next_in_data_pos-2)) == 0)
            Client->scan_state = PI_SCAN_WF_TAIL;
          else 
            Client->scan_state = PI_SCAN_WF_DATA;
	  break;

        case (PI_SCAN_WF_DATA) :
          Client->in_data[Client->next_in_data_pos] = temp_char;
          Client->next_in_data_pos++;
                   fprintf(stderr, "%x ", temp_char);
          if ((Client->in_data_len-(Client->next_in_data_pos-2)) == 0)
            Client->scan_state = PI_SCAN_WF_TAIL;
          else
            Client->scan_state = PI_SCAN_WF_DATA;
	  break;
 
        case (PI_SCAN_WF_TAIL) :
          Client->in_data[Client->next_in_data_pos] = temp_char;
          Client->scan_state = PI_SCAN_WF_HEAD;
          if (temp_char == 35) 
            return 2;
          else
            return 3;

        default :
          Client->scan_state = PI_SCAN_WF_HEAD;
	  break;
      } /* end switch */
    } /* end loop */
  } /* end ReadClientData */


 /*****************************************************************************
  * Name: int pack_ack_nack(unsigned char *target, int pacnum, int block_type *
  *                         int order_num)                                    *
  * Type: Function                                                            *
  *                                                                           * 
  * Purpose: The purpose of this function is to pack the ack/nack-block       *
  *          to send to MOTOR.                                                *
  *                                                                           *
  * Returnvalue: 0     -> Not done.                                           *
  *              n > 0 -> n bytes packed.                                     * 
  *                                                                           *
  * target: The target memory where to store the data.                        *
  * pacnum: The packnumber of the recieved block.                             *
  * block_type : The type of the block (ack or nack).                         *
  * order_num : The order number of the recieved block.                       *
  *****************************************************************************/

  int pack_ack_nack(unsigned char *target, int pacnum, int block_type, int order_num)
  {
    *target++ = HEAD_CHAR; /* The HEAD */
    *target++ = 4;
    *target++ = 0;
    *target++ = pacnum;
    *target++ = block_type;
    *target++ = order_num;
    *target = TAIL_CHAR; /* The TAIL */

    return 7;  
  } /* end pack_ack_nack */ 


 /**************************************************************************
  * Name: int get_next_pacnum(int con)                                     *
  * Type: Function                                                         *
  *                                                                        * 
  * Purpose: Find the next pacnum on the current socket.                   *
  *                                                                        *
  * Returnvalue: The new pacnumber.                                        *
  *                                                                        *
  * con: The connnection from where to find the pacnum.                    *
  **************************************************************************/

  int get_next_pacnum( struct socket_inf *Client)
  {
    if (Client->internal_pacnum < 255)
      Client->internal_pacnum++;
    else
      Client->internal_pacnum = 0;

    return Client->internal_pacnum;
  } /* end get_next_pacnum */


 /**************************************************************************
  * Name: int kill_pi()                                                    *
  * Type: Function                                                         *
  *                                                                        * 
  * Purpose: Find the next pacnum on the current socket.                   *
  *                                                                        *
  * Returnvalue: The new pacnumber.                                        *
  *                                                                        *
  * con: The connnection from where to find the pacnum.                    *
  **************************************************************************/

  void kill_pi( int code)
  {
	printf( "Ending PI execution\n");
	exit( 0 );
  }

  void MakeStatFile( int code )
  {
	printf( "User asked for current stats on pi performance\n");
	DumpStats( ALL_FUNCS );	
	if ( signal(SIGUSR2, MakeStatFile) == SIG_ERR)
		perror( "Can't install SIGUSR2\n " );
  }

void	RemoveLogFiles(void)
{
  if (!remove("pi_socket.log"))
    printf("PI-message: Logfile 'pi_socket.log' found and removed.\n");

  if (!remove("pi_event.log"))
    printf("PI-message: Logfile 'pi_event.log' found and removed.\n");

  if (!remove("pi_err.log"))
    printf("PI-message: Logfile 'pi_err.log' found and removed.\n");
}

void	InitSigHndlrs()
{
	int temp;

	for (temp=1; temp < NSIG; temp++)
		signal(temp, SIG_IGN);

	signal(SIGSEGV, SIG_DFL);
	signal(SIGPIPE, sig_noop);
	
	if ( signal(SIGUSR2, MakeStatFile) == SIG_ERR)
		perror( "Can't install SIGUSR2\n " );

/*	May need this latter
	if ( signal(SIGHUP, ParentHungUp ) == SIG_ERR)
		perror( "Can't install SIGHUP\n" );
*/
	if ( signal(SIGCHLD, death_process) == SIG_ERR)
		perror( "Can't install SIGCLD\n" );

	

	if ( signal(SIGTERM, KillProc) == SIG_ERR)
		perror( "Can't install SIGTERM\n" );
}

#ifdef FOO
 /**************************************************************************
  * Name: int SetUpListenSock( void )                                      * 
  * Type: Function                                                         *
  *                                                                        * 
  * Purpose:  Setup a socket to listen to clients .                        *
 **************************************************************************/

int	SetUpListenSock()
{
	int s;
  	struct protoent *prot_p;
  	struct sockaddr_in sin;

	/* Check if TCP is possible */
	if (!(prot_p = getprotobyname("ip"))) 
  	{
    	fprintf(stderr, "PI-error: Was not able to find the ip-protocol.\n");
    	exit(RESOURCE_PROBLEM);
  	}
  	printf("PI-message: IP-protocol have been found.\n");
  	endprotoent();
 
	/* Create socket, bind and listen to it */ 
  	if ((s = socket(AF_INET, SOCK_STREAM, 0 /*prot_p->p_proto*/)) < 0) 
  	{
    	fprintf(stderr,"PI-error: Was not able to use socket\n");
    	exit(RESOURCE_PROBLEM);
  	}
  	else
    	printf("PI-message: Socket 3000 have been selected.\n");

	if (getsockopt(s, SOL_SOCKET, SO_KEEPALIVE, NULL, NULL) < 0)
	  {
                fprintf(stderr,"**TCP** -- setsockopt failed");
		return(ERROR);
	  }
	else if (setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, NULL, NULL) < 0)
	  {
	    fprintf(stderr,"**TCP** -- setsockopt failed");
	    return(ERROR);
	  }
	
  	sin.sin_family        = AF_INET;
  	sin.sin_addr.s_addr   = htonl(INADDR_ANY);

	sin.sin_port          = htons(3000);

  	if (bind(s, (struct sockaddr*) &sin, sizeof(sin)) < 0) 
  	{
    	fprintf(stderr,"PI-error: Was not able to bind socket to address\n");
    	exit(RESOURCE_PROBLEM);  
  	}
  	else
    	printf("PI-message: Socket have been binded to address.\n");
  
  	if ( listen(s, 5) < 0 ) 	/* set limit for waiting connections */
  	{
    	fprintf(stderr,"PI-error: Can't listen on socket\n");
    	exit(RESOURCE_PROBLEM);  
  	}
	
	sprintf( szLogMsg, "SuperServer: Listening on %d socket\n", s );
	MakeLogEntry( szLogMsg );
	return s;
}
#endif

/************************************************************************

	Function: Create an ack message for API client request
	M.O.    : * bytes 1,2 - Return length
						* byte 3  --- Return clients pacnum ( matching API client request )
						* byte 4  --- Return ACK value
						* byte 5  --- return message type

						Save client parameters
						Write ACK to client socket

*************************************************************************/

void MakeAckMessage( unsigned char *temp_data,struct socket_inf *Client,
								struct return_struc return_data )
{
  //	sprintf( szMsgLog, "Sending ANSWER to client, data sz (%d)\n", return_data.data_len );
  //  sprintf( szMsgLog, "PI --> EXTP ANSWER_BLOCK_TO_CLIENT len=%d pacnum=%x order_num=%x\n" );
  //  MakeLogEntry( szMsgLog );

	/* Make message in temporary buffer */
	temp_data[0] = HEAD_CHAR;
	temp_data[1] = (return_data.data_len + 4) % 256;
	temp_data[2] = (return_data.data_len + 4) / 256;
	temp_data[3] = Client->this_pacnum;
	temp_data[4] = ANSWER_BLOCK_TO_CLIENT;
	temp_data[5] = Client->order_num; /* Ack for this message type */
	memcpy(&temp_data[6], return_data.data, return_data.data_len);
	temp_data[return_data.data_len + 6] = TAIL_CHAR;

	c_write( temp_data, return_data.data_len + 7);

        LogMsg( Client->order_num, PI_TO_EXTP, (char *)return_data.data );
        
	/* Save client parameters */
	memcpy(Client->out_data, temp_data, return_data.data_len +7);
	Client->try_send_out_data = 1;
	Client->last_out_pacnum = Client->this_pacnum;
	Client->out_data_len = return_data.data_len + 7;
}


void MakeOrderMessage( unsigned char *temp_data,struct socket_inf *Client,
								struct return_struc return_data )
{

	/* Make message in temporary buffer */
	temp_data[0] = HEAD_CHAR;
	temp_data[1] = (return_data.data_len + 4) % 256;
	temp_data[2] = (return_data.data_len + 4) / 256;
	temp_data[3] = Client->this_pacnum;
	temp_data[4] = ORDER_BLOCK_TO_CLIENT;
	temp_data[5] = Client->order_num; 
	memcpy(&temp_data[6], return_data.data, return_data.data_len);
	temp_data[return_data.data_len + 6] = TAIL_CHAR;

	c_write( temp_data, return_data.data_len + 7);

        LogMsg( Client->order_num, PI_TO_EXTP, (void *)return_data.data );
        
	/* Save client parameters */
	memcpy(Client->out_data, temp_data, return_data.data_len +7);
	Client->try_send_out_data = 1;
	Client->last_out_pacnum = Client->this_pacnum;
	Client->out_data_len = return_data.data_len + 7;
}


void  HandleClientMsg( unsigned char *temp_data, struct socket_inf *Client, clock_t StartTime )
{
	int nMsgNum;
	int	ack_nack_len, temp;
	struct order_struc order_data;
	struct return_struc return_data;
    short rv;
    char frame_type = '\0';

	/* Found bad state - no handling of message */
    if ( ( Client->illegal_block ) && ( Client->order_num != PI_LINE_MGR_ORDER ) )
	{
		sprintf( szLogMsg, 
			"<ORDER-BLOCK is found but with wrong length. pacnum = %d>\n", 
										Client->this_pacnum);
			MakeLogEntry( szLogMsg );
		return;
	}


	memcpy(order_data.data, &(Client->in_data[ORDER_DATA_START]), 
                             Client->in_data_len-4); 
	order_data.data_len = Client->in_data_len-4;
	nMsgNum = Client->order_num;

	sprintf(szLogMsg,"PI ORDER TYPE: %d", Client->order_num);
	MakeLogEntry(szLogMsg);
	
	switch (Client->order_num) 
	{
		case PI_UNKNOWN_ORDER:
			ERROR("PI have recieved an PI_UNKNOWN_ORDER from MOTOR");
			MakeStat( nMsgNum, (clock()-StartTime) );
			break;

		case PI_ZONE_ADDRESS:
			if (rv = pi_zone_address(&order_data, &return_data))
				MakeAckMessage( temp_data, Client, return_data );
			else
			{
				printf("PI-error: pi_zone_address failed with a returnvalue = 0.\n");
				exit(TAXI_ERROR);
			}
			MakeLogEntry( "After zone call\n" );
			MakeStat( nMsgNum, (clock()-StartTime) );
			break;

	  
		case PI_DISPATCH_CALL:
			if (pi_dispatch_call(&order_data, &return_data))
				MakeAckMessage( temp_data, Client, return_data );
			else
			{
				printf("PI-error: pi_dispatch_call failed with a returnvalue = 0.\n");
				exit(TAXI_ERROR);
			}
			MakeStat( nMsgNum, (clock()-StartTime) );
			break;

        case PI_COORD_TRANSFORM:
          if ( pi_coord_transform( &order_data, &return_data ) )
            {
              MakeAckMessage( temp_data, Client, return_data );
            }
          else
            {
              printf("PI-error: pi_coord_transform failed with a returnvalue = 0.\n");
              exit(TAXI_ERROR);
            }
          MakeStat( nMsgNum, (clock() - StartTime) );
          break;
          
		      case PI_DISPATCH_ACCOUNT_CALL:
		      case PI_DISPATCH_ACCOUNT_CALL2:
			if (pi_dispatch_account_call( &order_data, &return_data ) )
                          {
                            MakeAckMessage( temp_data, Client, return_data );
                          }
			else
			  {
			    printf("PI-error: pi_dispatch_account_call failed with a returnvalue = 0.\n");
			    exit(TAXI_ERROR);
			  }
			MakeStat( nMsgNum, (clock() - StartTime) );
			break;

		case PI_CANCEL_CALL:
			if (pi_cancel_call(&order_data, &return_data))
				MakeAckMessage( temp_data, Client, return_data );
			else
			{
				printf("PI-error: pi_cancel_call failed with a returnvalue = 0.\n");
				exit(TAXI_ERROR);
			}

			MakeStat( nMsgNum, (clock()-StartTime) );
			break;

		case PI_GET_CALL:
			if (pi_get_call(&order_data, &return_data))
				MakeAckMessage( temp_data, Client, return_data );
			else
			{
				printf("PI-error: pi_get_call failed with a returnvalue = 0.\n");
				exit(TAXI_ERROR);
			}
			MakeStat( nMsgNum, (clock()-StartTime) );
			break;

		case PI_GET_CALL_MESSAGE:
			if (pi_get_call_message(&order_data, &return_data))
				MakeAckMessage( temp_data, Client, return_data );
			else
			{
				printf("PI-error: pi_get_call_message failed with a returnvalue = 0.\n");
				exit(TAXI_ERROR);
			}
			MakeStat( nMsgNum, (clock()-StartTime) );
			break;                        

	case PI_GET_CALLH:
			if (pi_get_callh(&order_data, &return_data))
				MakeAckMessage( temp_data, Client, return_data );
			else
			{
				printf("PI-error: pi_get_callh failed with a returnvalue = 0.\n");
				exit(TAXI_ERROR);
			}
			MakeStat( nMsgNum, (clock()-StartTime) );
			break;
			
		case PI_GET_CAR:
			if (pi_get_car(&order_data, &return_data))
				MakeAckMessage( temp_data, Client, return_data );
			else
			{
				printf("PI-error: pi_get_car failed with a returnvalue = 0.\n");
				exit(TAXI_ERROR);
			}

			MakeStat( nMsgNum, (clock()-StartTime) );
			break;

		case PI_GET_EXCEPTIONS:
			sprintf( szLogMsg,"PI-message: PI_GET_EXCEPTIONS for Client on <con:%d>", Client->fd);
			MakeLogEntry( szLogMsg );
                            
			if (pi_get_exceptions(&order_data, &return_data))
				MakeAckMessage( temp_data, Client, return_data );
			else
			{
			  printf("PI-error: pi_get_exceptions failed with a returnvalue = 0.\n");
			  exit(TAXI_ERROR);
			}
			MakeStat( nMsgNum, (clock()-StartTime) );
			break;

		      case PI_ZONE_INFO:
			if (pi_zone_info(&order_data, &return_data))
			  MakeAckMessage( temp_data, Client, return_data );
			else
			  {
			    printf("PI-error: pi_report_exceptions failed with a return value = 0.\n");
			    exit(TAXI_ERROR);
			  }
			MakeStat( nMsgNum, (clock()-StartTime) );
			break;

	  
	case PI_RQST_GPS:
	  if ( pi_rqst_gps_pos( &order_data, &return_data) )
	    MakeAckMessage( temp_data, Client, return_data );
          else
            {
              printf("PI-error: pi_rqst_gps_pos failed with a return value = 0.\n");
              exit(TAXI_ERROR);
            }
          MakeStat( nMsgNum, (clock()-StartTime) );
          break;

	case PI_ACCOUNT_INFO:
	  if ( pi_account_validate(&order_data, &return_data) )
	    MakeAckMessage( temp_data, Client, return_data );
	  else
	    {
              printf("PI-error: pi_report_exceptions failed with a return value = 0.\n");
              exit(TAXI_ERROR);
            }
          MakeStat( nMsgNum, (clock()-StartTime) );
          break;

	case PI_GET_ACCOUNT_ADDR:
	  if ( pi_get_account_addr(&order_data, &return_data) )
	    MakeAckMessage( temp_data, Client, return_data );
	  else
	    {
              printf("PI-error: pi_get_account_addr failed with a return value = 0.\n");
              exit(TAXI_ERROR);
            }
          MakeStat( nMsgNum, (clock()-StartTime) );
          break;

	case PI_UPDATE_CALL:
	  if ( pi_update_call(&order_data, &return_data) )
	    MakeAckMessage( temp_data, Client, return_data );
	  else
	    {
              printf("PI-error: pi_update_call failed with a return value = 0.\n");
              exit(TAXI_ERROR);
            }
          MakeStat( nMsgNum, (clock()-StartTime) );
          break;	  

	case PI_GET_ACCOUNT_INDV:
	  if ( pi_get_account_indv(&order_data, &return_data) )
	    MakeAckMessage( temp_data, Client, return_data );
	  else
	    {
              printf("PI-error: pi_get_account_indv failed with a return value = 0.\n");
              exit(TAXI_ERROR);
            }
          MakeStat( nMsgNum, (clock()-StartTime) );
          break;	  
	  
	case PI_GET_ACCOUNT_CALLS:
	  if ( pi_get_account_calls(&order_data, &return_data) )
	    MakeAckMessage( temp_data, Client, return_data );
	  else
	    {
              printf("PI-error: pi_get_calls failed with a return value = 0.\n");
              exit(TAXI_ERROR);
            }
          MakeStat( nMsgNum, (clock()-StartTime) );
          break;
	  
        case PI_EMERG_PICTURE:
	  MakeLogEntry("PI_EMERG_PICTURE rcvd");

          if ( pi_emerg_picture(&order_data, &return_data) )
            MakeAckMessage( temp_data, Client, return_data );
          else
            {
              printf("PI-error: pi_report_exceptions failed with a return value = 0.\n");
              exit(TAXI_ERROR);
            }
          MakeStat( nMsgNum, (clock()-StartTime) );
          break;              
          
		      case PI_PRE_BOOK_INFO:
			if ( pi_pre_book_info( &order_data, &return_data ) )
			  MakeAckMessage( temp_data, Client, return_data );
			else
			  {
			    printf("PI-error: pi_pre_book_info failed with a return value = 0.\n");
			    exit(TAXI_ERROR);
			  }
			MakeStat( nMsgNum, (clock()-StartTime) );
			break;			  
 
		case PI_REPORT_EXCEPTIONS:
			if (pi_report_exceptions(&order_data, &return_data, 0, temp)) 
				/* set switch */
				MakeAckMessage( temp_data, Client, return_data );
			else
			{
			  printf("PI-error: pi_report_exceptions failed with a returnvalue = 0.\n");
			  exit(TAXI_ERROR);
			}
			MakeStat( nMsgNum, (clock()-StartTime) );
			break;

		case PI_ACCEPT_EXCEPTION:
			if (pi_accept_exception(&order_data, &return_data))
				MakeAckMessage( temp_data, Client, return_data );
			else
			{
			  printf("PI-error: pi_accept_exception failed with a returnvalue = 0.\n");
			  exit(TAXI_ERROR);
			}
			MakeStat( nMsgNum, (clock()-StartTime) );
			break;

		case PI_SEND_MESSAGE:
			if (pi_send_message(&order_data, &return_data))
				MakeAckMessage( temp_data, Client, return_data );
			else
			{
			  printf("PI-error: pi_send_message failed with a returnvalue = 0.\n");
			  exit(TAXI_ERROR);
			}
			MakeStat( nMsgNum, (clock()-StartTime) );
			break;

	case PI_VEH_STATUS:
	  if ( pi_veh_status(&order_data, &return_data))
            MakeAckMessage( temp_data, Client, return_data );
          else
            {
              printf("PI-error: pi_send_message failed with a returnvalue = 0.\n");
              exit(TAXI_ERROR);
            }
          MakeStat( nMsgNum, (clock()-StartTime) );
          break;
	  
        case PI_FLEET_STATUS:
          if (pi_fleet_status(&order_data, &return_data))
            MakeAckMessage( temp_data, Client, return_data );
          else
            {
              printf("PI-error: pi_send_message failed with a returnvalue = 0.\n");
              exit(TAXI_ERROR);
            }
          MakeStat( nMsgNum, (clock()-StartTime) );
          break;

        case PI_CBACK_COUT_ORDER:
          if ( pi_cback_cout_order( &order_data, &return_data ) )
            {
              MakeAckMessage( temp_data, Client, return_data );
            }
          else
            {
              printf("PI-error: pi_cback_cout_order failed with a return value = 0\n");
              exit(TAXI_ERROR);
            }
          MakeStat( nMsgNum, (clock()-StartTime) );
          break;
          
        case PI_LINE_MGR_ORDER:
          if (pi_line_mgr_order(&order_data, &return_data))
            MakeAckMessage( temp_data, Client, return_data );
          else
            {
              printf("PI-error: pi_line_mgr_order failed with a returnvalue = 0\n");
              exit(TAXI_ERROR);
            }
          MakeStat( nMsgNum, (clock() - StartTime) );
          break;
          
		case PI_GET_MESSAGE:
			if (pi_get_message(&order_data, &return_data))
				MakeAckMessage( temp_data, Client, return_data );
			else
			{
				printf("PI-error: pi_get_message failed with a returnvalue = 0.\n");
				exit(TAXI_ERROR);
			}
			MakeStat( nMsgNum, (clock()-StartTime) );
			break;

		case PI_ALIVE:
			if (pi_alive(&order_data, &return_data))
				MakeAckMessage( temp_data, Client, return_data );
			else
			{
			  printf("PI-error: pi_alive failed with a returnvalue = 0.\n");
			  exit(TAXI_ERROR);
			}
			MakeStat( nMsgNum, (clock()-StartTime) );
			break;

		case PI_QUIT:
			if (pi_quit(&order_data, &return_data))
				MakeAckMessage( temp_data, Client, return_data );
			else
			{
				printf("PI-error: pi_quit failed with a returnvalue = 0.\n");
				exit(TAXI_ERROR);
			}
			MakeStat( nMsgNum, (clock()-StartTime) );
			break;

		case PI_DISP_EXT_CALL:
			if (pi_dispatch_ext_call(&order_data, &return_data))
				MakeAckMessage( temp_data, Client, return_data );
			else
			{
				printf("PI-error: pi_dispatch_ext_call failed with a returnvalue = 0.\n");
				exit(TAXI_ERROR);
			}
			MakeStat( nMsgNum, (clock()-StartTime) );
			break;

		case PI_GENERATE_EXCEPTION:
			if (pi_generate_exception(&order_data, &return_data))
				MakeAckMessage( temp_data, Client, return_data );
			else
			{
				printf("PI-error: pi_generate_exception failed with a returnvalue = 0.\n");
				exit(TAXI_ERROR);
			}
			MakeStat( nMsgNum, (clock()-StartTime) );
			break;

		default: /* USED WHEN THE ORDERNUMBER IS UNKNOWN */
			temp_data[0] = HEAD_CHAR;
			temp_data[1] = 5;
			temp_data[2] = 0;
			temp_data[3] = get_next_pacnum(Client);
			temp_data[4] = 6; /* An orderblock send from PI to MOTOR */
			temp_data[5] = PI_UNKNOWN_ORDER;
			temp_data[6] = Client->order_num;
			temp_data[7] = TAIL_CHAR;
			c_write( temp_data, 8);
			memcpy(Client->out_data, temp_data, 8);
			Client->try_send_out_data = 1;
			Client->last_out_pacnum = temp_data[3];
			Client->out_data_len = 8;

	} /* end switch */
}

void HandleClientAccountMsg( unsigned char *temp_data, struct socket_inf *Client, clock_t StartTime )
{
  int nMsgNum;
  int ack_nack_len, temp;
  struct order_struc order_data;
  struct return_struc return_data;
  short rv;

  if (Client->illegal_block)
    {
      sprintf(  szLogMsg, 
			"<ORDER-ACCT-BLOCK is found but with wrong length. pacnum = %d>\n", 
										Client->this_pacnum);
			MakeLogEntry( szLogMsg );
		return;
    }

  memcpy( order_data.data, &(Client->in_data[ORDER_DATA_START]),
	 Client->in_data_len-4);
  order_data.data_len = Client->in_data_len-4;
  nMsgNum = Client->order_num;

  pi_dispatch_account_call( &order_data, &return_data );
  MakeAckMessage( temp_data, Client, return_data );
  
}

void HandleLineMgrCback()
{
}

void HandleLineMgrCout()
{
}

int	HandleClient()
{
	unsigned char temp_data[MAX_DATA_LEN];
	int   ack_nack_len;
	clock_t StartTime;

	StartTime = clock();

	switch ( ReadClientData( &ClientSock) )
    {
		case (2) : /* ORDER-block is found and ready (tail is ok) */
			if (ClientSock.block_type == 0)
					HandleClientMsg( temp_data, &ClientSock, StartTime );
			else if (ClientSock.block_type == 7) /** Account call order **/
			  HandleClientAccountMsg( temp_data, &ClientSock, StartTime );
			break;
         
		case (3) : /* ORDER-block is found but not ready (wrong tail) */
			ack_nack_len = pack_ack_nack(temp_data, ClientSock.this_pacnum,
                      								3, ClientSock.order_num);
			c_write( temp_data, ack_nack_len);
			break;
    case (-1) :
      close(ClientSock.fd);
      return(-1);
      break;

	} /* end switch */
	return(1);
} 



void  CheckHbStats()
{
	GenExcept  sException;
	int       x;


	for ( x=0; x < nChildren; x++ )
	{
		if ( !ServChildInfo[x].bResponded
					&& ServChildInfo[x].lLastHeartBeat - time(NULL) )
		{
			sprintf( szLogMsg, "Child Server pid (%d) not responding", ServChildInfo[x].nPid );
			MakeLogEntry(szLogMsg);
			if ( ( ServChildInfo[x].lLastHeartBeat - time(NULL) ) > 2*HB_TO_SERVER_TIME   )
				MakeSysExcept(ServChildInfo[x].nPid,API_CHILD_DEATH);
		}
		write( ServChildInfo[x].nHeartBeatPipe, BEAT, sizeof(BEAT) );
	}

}

void  MakeHbToServers()
{
	int       x;

	for ( x=0; x < nChildren; x++ )
		if ( ServChildInfo[x].nHeartBeatPipe > 0 )
			write( ServChildInfo[x].nHeartBeatPipe, BEAT, strlen(BEAT) );

}

void	KillProc( int code )
{
	if ( bSuperServer )
	{
		printf( "Exiting Killed by SIGTERM signal\n" );
		kill( 0, SIGTERM );
		exit(SIGNAL_EXIT);
	}

	printf( "Child exiting by SIGTERM signal\n" );
	exit(SIGNAL_EXIT);
		
}


 /**************************************************************************
  * Name: void main(int argc, char **argv)                                 * 
  * Type: Function                                                         *
  *                                                                        * 
  * Purpose: To run the pi-module.                                         *
  **************************************************************************/

void main(int argc, char **argv) 
{
  int x, s, len, alive_time;
  int rc = 0;
  struct sockaddr from;
  struct order_struc order_data;
  struct return_struc return_data;
  unsigned char temp_data[MAX_DATA_LEN];
  unsigned char temp_string[256];
  struct timeval select_timeout;
  struct tm 		*cur_time;
  time_t      lTimeInSecs;
  time_t last_msg_time;
  char  HbBuf[10];
  
  fd_set fd_set_read;
  int select_result;
  char sLocale[80];
  char    CatPathName[64];        
  
  RemoveLogFiles();
  
  printf("\n\n\n\nWelcome to TaxiPak's PI 4.5   2001-Oct-28 (c)1997-2001 MobileSoft Consulting, Inc.\n\n");
  
  
  lTimeInSecs = time( NULL );
  cur_time = localtime( &lTimeInSecs );
  sprintf(szLogMsg, "Date: %s\n", asctime( cur_time ) );
  MakeLogEntry( szLogMsg );
  
  
  set_options(argc, argv);
  
  if (enh(&glEnhSwitch) < 0)
    {
      /* Error logging */
      exit( -1 );
    }
  
  strcpy( sLocale, TPAK_LOCALE );	
  setlocale(LC_ALL,"" );
  
  sprintf( CatPathName, "%s%s.cat", TPAK_NLS_PATH, "common" );
  COMMON_m_catd = catopen( CatPathName, 0 );  
  sprintf( CatPathName, "%s%s.cat", TPAK_NLS_PATH, "ui" );
  UI_m_catd = catopen( CatPathName, 0 );
  sprintf( CatPathName, "%s%s.cat", TPAK_NLS_PATH, "writer" );
  WRITER_catd = catopen( CatPathName, 0 );
  
  open_catalog(&UI_m_catd, "ui", 0, 0);
  
  
  printf("PI-message: Using monitor (1=YES, 0=NO) %d.\n",using_monitor);
  
  if ( (msgkey = msgget(PI_MSGKEY,IPC_CREAT|0666)) == -1 )  {
    /* msgget failed */
    exit(-1);
  }

  if ( (main_msgkey = msgget(MSGKEY,IPC_CREAT|0666)) == -1 )  {
    /* msgget failed */
    exit(-1);
  }  

#ifdef FOO
  if (!init_pi())
    {
      fprintf(stderr, "PI-error: Init_pi failed, no monitor found.\n");
      exit(1);
    }
#endif

    Zone_mem_init();
    
  printf("PI-message: Main part of PI have been initialized.\n");
  
  init_con( &ClientSock );	/* Init msg/comm parameters */
  init_action();			/* Init exception report structs */
  
  
  // Make shared memory segment for Line_Manager
  m_MakeSharedMemory();
  
  printf( "PI-message: Socket_connection (Service Port 3000) initialized.\n");
  
  ListenSock = SetUpListenSock(3000); 	/* SuperServer list for requests */
  
  FD_ZERO( &ReadMask );
  FD_SET( ListenSock, &ReadMask );
  
  InitChildInfo();							/* SuperServerList of Server&Sockets */
  
  if ( !bSuperServer )
    {
      select_timeout.tv_sec = 0;
      select_timeout.tv_usec = 500000;
      alive_time = 0;
    }
  else
    {
      select_timeout.tv_sec = HB_TO_SERVER_TIME;
      select_timeout.tv_usec = 0;
      alive_time = 0;
    }
  
  
  InitStats();	/* Stats for timing code fragments */
  InitSigHndlrs();
  
  printf("\nPI-message: Starting the PI process for use on TaxiPak.\n\n");
  
  /* Main loop to process -client data, heartbeats, reports */
  int ended = 0;
  while(!ended) 
    {

        
      FD_ZERO(&ReadMask);
      FD_SET( ListenSock, &ReadMask );
      
      fd_set_read = ReadMask;
      
      switch(select(FD_SETSIZE, &fd_set_read, NULL, NULL, &select_timeout))
        {
        case (-1):
          if ( bSuperServer  && using_monitor )
            signal_to_monitor();   

          break;
          
        case (0):
				/* Signal to Monitor process ! */
          if (bSuperServer)
            {
              if (using_monitor == 1)
                signal_to_monitor();
              
              break;
            }
	  else
	    {
	      if ( time(0) - last_msg_time > 28800 ) // more than 8 hours since last message activity
		{
		  close( ClientSock.fd ); // close the socket connection
		  exit(0); // terminate the server process
		}
	    }	  
          
				/* Servers make exception report to clients */
#ifdef FOO
	  // Something wacky was happening with the broadcast of exceptions....shut it down...nobody uses it
          if (pi_report_exceptions(&order_data, &return_data, 1)) /* set switch */
            {
              temp_data[0] = HEAD_CHAR;
              temp_data[1] = (return_data.data_len + 4) % 256;
              temp_data[2] = (return_data.data_len + 4) / 256;
              temp_data[3] = get_next_pacnum( &ClientSock );
              temp_data[4] = 1; /* An answerblock send from PI to MOTOR */
              temp_data[5] = PI_REPORT_EXCEPTIONS;
              memcpy(&temp_data[6], return_data.data, return_data.data_len);
              temp_data[return_data.data_len + 6] = TAIL_CHAR;
              c_write(  temp_data, return_data.data_len + 7);
              
              memcpy( ClientSock.out_data, temp_data, return_data.data_len +7);
              ClientSock.try_send_out_data = 1;
              ClientSock.last_out_pacnum = temp_data[3];
              ClientSock.out_data_len = return_data.data_len + 7;
            } /* end if pi_report_exceptions */
          
          select_timeout.tv_sec = 0;
          select_timeout.tv_usec = 500000;
          alive_time = 0;
#endif
          
          
          break;
          
        default: /* A socket is ready for reading */
          
          if ( bSuperServer && using_monitor == 1)
            signal_to_monitor();
          
          if (bSuperServer )  /* Check only listen Socket */
            {
              if ( FD_ISSET(ListenSock, &fd_set_read) )
		{
		  last_msg_time = (time_t)time(0);
		  handle_new_socket(ListenSock, &ReadMask);
		}
            }
          else      /* Check single client */
            {

              if ( FD_ISSET(ClientSock.fd, &fd_set_read) )
		{
		  last_msg_time = (time_t)time(0);
		  if ((rc = HandleClient()) < 0)
		    {
		      kill(getpid(),SIGKILL);
		      ended = 1;
		      break;
		    }
		}
              else
		{

		}
	      
              if ( FD_ISSET( HbSocket[CHILD_HB_SOCK], &fd_set_read ) )
                {
                  read( HbSocket[CHILD_HB_SOCK], HbBuf, sizeof(HbBuf) );
                  write( HbSocket[CHILD_HB_SOCK], BEAT, sizeof(BEAT) );
                }
            }
          if ( !bSuperServer )  /* New clients get time initialized this way */
            {
              select_timeout.tv_sec = 0;
              select_timeout.tv_usec = 500000;
              alive_time = 0;
            }
          
          CheckChildren( 0 );
          
          break;
          
        } /* end switch */ 
      
    } /* end while(1) */
} /* end main() */

/****************************************************
	Kevin's partition
****************************************************/
void	MakeStat( int nFunc, clock_t lDuration )
{
	int	nSlot;

	nSlot = GET_STATSLOT(nFunc) = ++GET_STATSLOT(nFunc) % MAX_SAMPLES; 

	GETSTAT(nFunc, nSlot) = lDuration;

	/* Check Peak value */
	if ( GETSTAT(nFunc, nSlot) > GETSTAT(nFunc, PEAK_TIME) 
			|| !GETSTAT(nFunc, nSlot ) )
		GETSTAT(nFunc, PEAK_TIME) = lDuration;

	/* Check Min value */
	if ( GETSTAT(nFunc,nSlot) < GETSTAT(nFunc, MIN_TIME) 
			|| !GETSTAT(nFunc, MIN_TIME ) ) 
		GETSTAT(nFunc,MIN_TIME) = lDuration;
}

int	DumpStats( int nFunc )
{
	FILE	*fpStats;
	int		i;
	struct tms *bufTms;
	clock_t	csTime;

	fpStats = fopen( STAT_FILE, "w" );
	if ( fpStats == NULL )
	{
		perror("Can't open stat file" );
		return FALSE;
	}

	/*csTime = times( bufTms ); */
	fprintf( fpStats, "Report on function statistic\n\n" );
/*	fprintf( fpStats, "PI running for %d secs\n\n", 0 ); */
	if ( nFunc == ALL_FUNCS )
		for( i=0; i < NUM_STAT_FUNCS; i++ )
			DumpFuncStats( i, fpStats );	
	else if ( nFunc < 0 || nFunc >= NUM_STAT_FUNCS )
		printf( "Illegal func for stats\n" );
	else
		DumpFuncStats( nFunc, fpStats );
	
	fclose( fpStats );
	printf( "Dumped statistics to %s\n", STAT_FILE );
  	InitSigHndlrs();
}

void InitStats(void)
{
	int i,nFunc;

	for ( nFunc=0; nFunc < NUM_STAT_FUNCS; nFunc++)
		for ( i = 0; i < SAMPLE_SPACE; i++ )
			GETSTAT( nFunc, i ) = 0;
}

void DumpFuncStats(int nFunc, FILE *fpStats )
{
	int	i;

	fprintf( fpStats, "Function : %s\n", szStatFuncs[nFunc] );
	for( i=0; i < MAX_SAMPLES; i++ )
	{
		if ( !GETSTAT( nFunc, i )  )
			fprintf( fpStats, "Entry %d : N/A\n", i );
		else 
			fprintf( fpStats, "Entry %d, %d (ms)\n",  
							i, (int)GETSTAT( nFunc, i )/1000 );
	} 
	
	if ( !GETSTAT( nFunc, MIN_TIME ) )	
		fprintf( fpStats, "Minimum : N/A\n" );
	else
		fprintf( fpStats, "Minimum : %d (ms)\n", 
						(int)GETSTAT( nFunc, MIN_TIME)/1000 );  

	if ( !GETSTAT( nFunc, PEAK_TIME ) )	
		fprintf( fpStats, "Max : N/A\n" );
	else
		fprintf( fpStats, "Max : %ld (ms)\n", 
						(int)GETSTAT( nFunc, PEAK_TIME)/1000 );  
}

int		HandleStatSig( int code )
{
	DumpStats( ALL_FUNCS );	
}

void sig_noop()
{
}

m_MakeSharedMemory()
{
  int     size, i,val;
  LINE_MGR_STRUCT *pLineMgrShmem, *pLineMgr;
  TAO_QUEUE_STRUCT    *pCallQueueShmem;
  char      *tmp_shmptr;

  size = sizeof( struct LineMgr_offsets )
    + MAX_TTYS * sizeof( LINE_MGR_STRUCT )
      + QUEUE_SIZE * sizeof( TAO_QUEUE_STRUCT );

  if ((LineMgrshmid = shmget(LINE_MGR_SHMEM_KEY, size, 0777 | IPC_CREAT)) == -1)
    {
      perror( "MakeSharedMemory - couldn't get shared memory id");
      exit(-1);
    }

  LineMgrshmptr = shmat(LineMgrshmid, 0, 0);
  memset(LineMgrshmptr, '\0', size);
  pLineMgr_offset = ( struct LineMgr_offsets * ) LineMgrshmptr;

  tmp_shmptr = (char *) ++pLineMgr_offset;
  pLineMgr_offset->Line_Mgr_Array = (LINE_MGR_STRUCT *) tmp_shmptr;
  pLineMgrShmem = (LINE_MGR_STRUCT *) tmp_shmptr;
  for (i = 0; i < MAX_TTYS; i++)
    {
      Lines[ i ] = pLineMgrShmem++;
    }
  pCallQueueShmem = (TAO_QUEUE_STRUCT *)pLineMgrShmem;
  for (i = 0; i < QUEUE_SIZE; i++)
    {
      TaoCallQueue[ i ] = pCallQueueShmem++;
    }
  
  pLineMgr_offset->init_time = time(0);
  
  return;
}

death_process() 
{
        int pid;
	long msgtype = 0;
	int rc = 0;
        int status; 
        struct rusage usage; 

        while ((pid=wait3(&status,WNOHANG,&usage)) > 0) 
	{

	}
	//	siglongjmp(xts_jmpbuf, 1);
			
}

int LostConnect()
{
   close( ListenSock );
  exit(-1);  
}

void Zone_mem_init()
{
  int fl_idx = 0, zone_nbr = 42, point_index = 0, k;
  ZONE_POLY_REC zonepoly_rec;
  int mode = ISEQUAL, done = 0;
  short zn_nbr = 1;
  MEM_ZONE *pZone;

  db_open( ZONEPOLY_FILE_ID, DB_READ_ONLY|DB_MANUAL_LOCK );
  for ( fl_idx = 7; fl_idx >= 7; fl_idx-- )
    {

      for ( k = 1; k < ZONE_MAX; k++ )
	{
	  mode = ISEQUAL; done = 0; zonepoly_rec.fleet = 0x41 + fl_idx; zonepoly_rec.nbr = k; zonepoly_rec.seq_nbr = 1;point_index = 0;
	  while ( !done )
	    {
	      if ( db_read_key(ZONEPOLY_FILE_ID, &zonepoly_rec, &znpoly_key1, mode ) == SUCCESS )
		{
		  if ( mode == ISEQUAL ) // first point in zone...malloc
		    {
		      pZone = (MEM_ZONE *)malloc( sizeof( MEM_ZONE ) );
		      fleetMemZones[fl_idx][k] = (MEM_ZONE *)pZone;
		    }
		  
		  mode = ISNEXT;
		  if ( zonepoly_rec.nbr != k )
		    done = TRUE;
		  else
		    {
		      pZone->zone_poly[point_index].x = zonepoly_rec.pointx;
		      pZone->zone_poly[point_index].y = zonepoly_rec.pointy;
		      ++point_index;
		      
		      if ( point_index > MAX_POLY_POINTS )
			break;
		    }
		}
	      else
		done = 1;
	    }  // loop zonepoly point
	  if (iserrno != 111)
	    pZone->poly_points = point_index;
	  if ( point_index > 0 )
	    ZonePolyCalculateCentroid( pZone );
	} // next zone
      
    } // next fleet
  

  db_close( ZONEPOLY_FILE_ID );
}
