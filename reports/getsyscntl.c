static char sz__FILE__[]="@(#)getsyscntl.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  getsyscntl.c; Rel: 10.1.0.0; Get date: 4/18/95 at 17:02:11
 * @(#)  Last delta: 12/2/94 at 11:34:46
 * @(#)  SCCS File: /taxi/sccs/s.getsyscntl.c
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include <string.h>

#include <isam.h>
#include <sys/types.h>
#include "mads_types.h"
#include "ex_key.h"		/* define external db keys */
#include "df_maint.h"		/* for defines */
#include "mads_isam.h"		/* for defines */
#include "rep_strdefs.h"

extern	char buf[];	/* return buffer for call record */
extern	char req_buf[];	/* buffer to cast as call structure */
extern	int db();

/*====================================================================
= get_syscntl() -
=	Returns the record in the system control data base.
=	Who:	Bruce Young
=	What:	Written		When: 2 March 1988
=====================================================================*/
int get_syscntl(sys_day_start,sys_day_end,sys_prod_start_time,sys_gen_rpt1_n,sys_print_rpt1_n,keep_data)
/* integer data area is automatically allocated by the calling process */
int *sys_day_start,*sys_day_end,*sys_prod_start_time;
/* character string storage must be allocated by the calling processs */
char *sys_gen_rpt1_n,*sys_print_rpt1_n;
int * keep_data;
{
	int ccode;
	char *p1;
	struct	cisam_syc *systemrec;
	systemrec=(struct cisam_syc*)req_buf;	/* to cast req_buf */

	/* initialize to read the cisam SYSTEM CONTROL file */
	ccode=db(SYSCNTL_FILE_ID,OPEN,&fl_key2,ISINPUT | ISMANULOCK,0);	/* open CALL db for read only */
	if (ccode<0) {
		printf(catgets(REP_catd, 1, 1, "Error OPENing file %s iserrno %d\n"),SYSCNTL_FILE_ID,iserrno);
		exit(-1);
	}	/* end if ccode */

	/* ISFIRST points to the first, and only, record in the CISAM file */
	ccode=db_read(SYSCNTL_FILE_ID, systemrec, ISFIRST);
	if (ccode<0) {
		printf(catgets(REP_catd, 1, 2, "Error READing file %s iserrno %d\n"),SYSCNTL_FILE_ID,iserrno);
		exit(-1);
	}	/* end if ccode */

	*keep_data=systemrec->keep_data_for_n_days;
	*sys_day_start=systemrec->day_start;
	*sys_day_end=systemrec->day_end;
	*sys_prod_start_time=systemrec->prod_start_time;
	strcpy(sys_gen_rpt1_n,systemrec->gen_rpt1_n);
	strcpy(sys_print_rpt1_n,systemrec->print_rpt1_n);

	/* close the SYSTEMCONTROL file */
	ccode=db(SYSCNTL_FILE_ID,CLOSE,&fl_key2,0,0);
	if (ccode<0) {
		printf(catgets(REP_catd, 1, 3, "Error CLOSEing file %d iserrno %d\n"),SYSCNTL_FILE_ID,iserrno);
		exit(-1);
	}	/* end if ccode */
#ifdef DEBUG
	printf("systemrec->day_start %d systemrec->day_end %d systemrec->prod_start_time %d \n", 
	systemrec->day_start,systemrec->day_end,systemrec->prod_start_time);
	printf("systemrec->gen_rpt1_n %s systemrec->print_rpt1_n %s\n",systemrec->gen_rpt1_n,systemrec->print_rpt1_n);
	printf("sys_day_start %d sys_day_end %d sys_prod_start_time %d \n",*sys_day_start,*sys_day_end,*sys_prod_start_time);
	printf("sys_gen_rpt1_n %s sys_print_rpt1_n %s\n",sys_gen_rpt1_n,sys_print_rpt1_n);
#endif
	return(ccode);
}	/* end sys_cntl */
