/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: requests.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1997/08/09 17:57:53 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/requests.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: requests.h,v $	$Revision: 1.2 $"

#define MAXBUFFER	500

typedef struct msgqrec
{
        long	r_mtype;
        int  	r_rtype;
	int	r_pid;
	int 	r_seq;
	int	r_size;
        char 	r_buf[MAXBUFFER];
} REQUESTDEF;

#define QREQUESTS            1
#define QRESPONDS	     2
#define PROCESSCHECK	     3
