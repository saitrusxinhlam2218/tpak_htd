static char sz__FILE__[]="@(#)da_usesum.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  da_usesum.c; Rel: 5.1.0.0; Get date: 2/27/90 at 12:21:20
 * @(#)  Last delta: 12/14/89 at 17:00:52
 * @(#)  SCCS File: /taxi/sccs/s.da_usesum.c
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

extern	char buf[];	/* for db put and get records       */
#include <fcntl.h>
#include <isam.h>
#include <sys/types.h>
#include "mads_types.h"
#include "key.h"
#include "rep_strdefs.h"
#include "path.h"

extern	char buf[];	/* for db put and get records       */
extern	char req_buf[];	/* buffer to cast as call structure */
extern	int db();

/* for the shared memory processes */
#include "mad_ipc.h"
/* define the structures to attach memory */
struct	offsets *offset;
extern struct	offsets *get_offsets();

struct	cisam_cl *shiftrec;
/* define a call record pointer */
struct	cisam_cl *callrec;
/* define a history record pointer */
struct	cisam_ch *histrec;

/*=====================================================================*/
	char *fleet_id=argv[1];
	register int ccode;			/* returned from proc calls */
	register int indx;
	int fleets=strlen(fleet_id);		/* must follow the definition of fleet_id */
	long ending_dt_tm=atol(argv[2]);	/* should be at least 24 hours before the current time */

	/* attach shared memory and print redundant record transactions */
	offset = get_offsets();

	/* cast the buffers as the request buffer */
	callrec=(struct cisam_cl*)req_buf;
	histrec=(struct cisam_ch*)req_buf;

	/* open two tables: CALL and CALL HISTORY */
	/*  open the cisam CALL file, using the composite key3: fleet, due_date_time */
	if (db(CALL_FILE_ID,OPEN,&cl_key3,ISINOUT | ISMANULOCK,0)<0) {
		/* open CALL db for read only */
		printf(catgets(REP_catd, 1, 1, "Error OPENing table %d iserrno %d\n"),CALL_FILE_ID,iserrno);
		return(-1);
	}	/* end if ccode */
#ifdef DEBUG
printf("working with: fleet_id=%c ending_dt_tm=%d\n",fleet_id[indx],ending_dt_tm);
#endif
		/* ISGTEQ points to the first record matching fleet with time now or later */
		callrec->fleet=fleet_id[indx]; 
		/* the starting time 0 should used to remove any older records. */
		callrec->due_date_time = 0;
		ccode=db(CALL_FILE_ID,READ_KEYS,&cl_key3,ISGTEQ,0);
#ifdef DEBUG
printf("call table: the first record returns ccode = %d\n",ccode);
#endif
	/*  The previous shift calls were removed from the CALL file. Now, close that file. */
	if (db(CALL_FILE_ID,CLOSE,&cl_key1,0,0)<0) {
		printf(catgets(REP_catd, 1, 2, "Error CLOSEing table %d iserrno %d\n"),CALL_FILE_ID,iserrno);
		exit(-1);
	}	/* end if ccode */
	shmdt(offset);
	exit(0);
}	/* end of da_purge */

/*====================================================================
= da_useput()
=	reads daily summary statistics from standard input
=	adding a record to the statistics table.
=	Who:	Bruce Young
=	What:	Written		When: 20 August 1988
=====================================================================*/
main(argc,argv)
int argc;
char *argv[];
{
	char working_fleet=argv[1];

	int ccode;		/* returned from proc calls */
	int fleetno;
	int report_no=15;
	register int charge_counter, call_counter;
	char buffer[264];
	char systembuff[256];

		/* initialize the shiftrec structure */
		shiftrec=(struct cisam_cl*)req_buf;	/* to cast req_buf */
		working_fleet = fleet_id[fleetno];
		shiftrec->fleet = working_fleet;	/* first key field */
		shiftrec->due_date_time = 0;		/* second key field */
		call_counter=0;
		charge_counter=0;

		/* open the cisam SHIFT file, using the composite key2, fleet & due_dt_tm */
		ccode=db(SHIFT_FILE_ID,OPEN,&sh_key2,ISINPUT | ISMANULOCK,0);	/* open CALL db for read only */
		if (ccode<0) {
			printf(catgets(REP_catd, 1, 3, "Error OPENing file %d iserrno %d\n"),SHIFT_FILE_ID,iserrno);
			exit(-1);
		}	/* end if ccode */

		ccode=db(SHIFT_FILE_ID,READ_KEYS,&sh_key2,ISGTEQ,0);
		while (ccode>=0 && shiftrec->fleet==working_fleet) { 
			/* continue while matching fleet */

			/* count all calls */
			call_counter++;
			/* non-NULL must have a charge number */
			if (strlen(shiftrec->charge_nbr)>0) {
				charge_counter++;
#ifdef DEBUG
		fprintf(fp,"fleet %c call_nbr %d charge_nbr %s account_name %s extended_type %s\n",
		shiftrec->fleet, shiftrec->nbr, shiftrec->charge_nbr, shiftrec->company_name, shiftrec->extended_type);
#endif
			}	/* end if */

			ccode=db(SHIFT_FILE_ID,READ_KEYS,&sh_key2,ISNEXT,0);	/* get next record */
		}	/* end of matching fleet_id */
		ccode=db(SHIFT_FILE_ID,CLOSE,&sh_key2,ISNEXT,0);

}	/* end of da_useput */
