 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  init.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:46:43
 * @(#)  Last delta: 12/2/94 at 18:16:01
 * @(#)  SCCS File: /taxi/sccs/s.init.h
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:init.h	10.1.2.1"

struct config {
	char module_name[8];		/* 333 load module name		*/
	short CN_vrtx_cpu_num; 		/* VME 333 controller number	*/
	short CN_vrtx_ip;               /* VRTX based Interface point	*/
	short CN_port_num;  		/* 0 - 5			*/

	short  buff_size;		/* 256,400,512, ...		*/
	short  block_factor;		/* Number of blocks to send	*/

	short  bid_limit;		/* bid ENQs	 		*/
	short  nak_limit;		/* # of consecutive naks	*/
	short  wack_limit;		/* # of consecutive wacks	*/
	short  bad_limit;		/* # of consecutive bad resp	*/
	short  no_resp_limit;		/* # of consecutive no resp	*/

/* 	50 milliseconds / tick	*/

	short  cts_timeout;		/* #ticks for cts		*/
	short  bid_timeout;		/* #ticks between bids 		*/
	short  resp_timeout;		/* #ticks for a response 	*/
	short  wack_timeout;		/* #ticks delay on wack	 	*/
	short  text_timeout;		/* #ticks for text 		*/
	short  idle_timeout;		/* #ticks for idle line		*/
	short  sleepy_time;		/* #ticks for sleepy_time	*/
	short	RS232_time;		/* #ticks 232 change tolerance	*/
	unsigned char  xmit_bufs;	/* Number of xmit buffers 	*/
	unsigned char  recv_bufs;	/* Number of recv buffers 	*/
	unsigned char  logon;		/* Initial logging state 	*/
	unsigned char  fulldx;		/* Keep RTS UP?		 	*/
        };
