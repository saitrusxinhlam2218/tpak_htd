static char sz__FILE__[]="@(#)fill_syc.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_syc.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:38:28
 * @(#)  Last delta: 12/2/94 at 11:32:43
 * @(#)  SCCS File: /taxi/sccs/s.fill_syc.c
 *                                                                           *
 *****************************************************************************/

#define KEY_SRC
#include "System_control_db.h"

void
fill_syc_key(req_buf, buf)
     char *req_buf;
     char *buf;
{

}


fill_syc(req_buf, buf, action)
     char *req_buf;
     char *buf;
     int   action;
{
	struct  cisam_syc  *syc;
	register int  offset = 0;

	syc = (struct cisam_syc*)req_buf;

        stint(syc->day_start,&buf[offset]);
        offset += INTSIZE;

        stint(syc->day_end,&buf[offset]);
        offset += INTSIZE;

        stint(syc->prod_start_time,&buf[offset]);
        offset += INTSIZE;

	offset += sizeof(syc->fleet0);
	offset += sizeof(syc->index0);
	stchar(syc->gen_rpt0_n,&buf[offset],sizeof(syc->gen_rpt0_n)-1);
        offset += sizeof(syc->gen_rpt0_n)-1;
	stchar(syc->print_rpt0_n,&buf[offset],sizeof(syc->print_rpt0_n)-1);
        offset += sizeof(syc->print_rpt0_n)-1;

	offset += sizeof(syc->fleet0);
	offset += sizeof(syc->index0);
	stchar(syc->gen_rpt1_n,&buf[offset],sizeof(syc->gen_rpt1_n)-1);
        offset += sizeof(syc->gen_rpt1_n)-1;
	stchar(syc->print_rpt1_n,&buf[offset],sizeof(syc->print_rpt1_n)-1);
        offset += sizeof(syc->print_rpt1_n)-1;

	offset += sizeof(syc->fleet0);
	offset += sizeof(syc->index0);
	stchar(syc->gen_rpt2_n,&buf[offset],sizeof(syc->gen_rpt2_n)-1);
        offset += sizeof(syc->gen_rpt2_n)-1;
	stchar(syc->print_rpt2_n,&buf[offset],sizeof(syc->print_rpt2_n)-1);
        offset += sizeof(syc->print_rpt2_n)-1;

	offset += sizeof(syc->fleet0);
	offset += sizeof(syc->index0);
	stchar(syc->gen_rpt3_n,&buf[offset],sizeof(syc->gen_rpt3_n)-1);
        offset += sizeof(syc->gen_rpt3_n)-1;
	stchar(syc->print_rpt3_n,&buf[offset],sizeof(syc->print_rpt3_n)-1);
        offset += sizeof(syc->print_rpt3_n)-1;

	offset += sizeof(syc->fleet0);
	offset += sizeof(syc->index0);
	stchar(syc->gen_rpt4_n,&buf[offset],sizeof(syc->gen_rpt4_n)-1);
        offset += sizeof(syc->gen_rpt4_n)-1;
	stchar(syc->print_rpt4_n,&buf[offset],sizeof(syc->print_rpt4_n)-1);
        offset += sizeof(syc->print_rpt4_n)-1;

	offset += sizeof(syc->fleet0);
	offset += sizeof(syc->index0);
	stchar(syc->gen_rpt5_n,&buf[offset],sizeof(syc->gen_rpt5_n)-1);
        offset += sizeof(syc->gen_rpt5_n)-1;
	stchar(syc->print_rpt5_n,&buf[offset],sizeof(syc->print_rpt5_n)-1);
        offset += sizeof(syc->print_rpt5_n)-1;

	offset += sizeof(syc->fleet0);
	offset += sizeof(syc->index0);
	stchar(syc->gen_rpt6_n,&buf[offset],sizeof(syc->gen_rpt6_n)-1);
        offset += sizeof(syc->print_rpt6_n)-1;

	offset += sizeof(syc->fleet0);
	offset += sizeof(syc->index0);
	stchar(syc->gen_rpt7_n,&buf[offset],sizeof(syc->gen_rpt7_n)-1);
        offset += sizeof(syc->gen_rpt7_n)-1;
	stchar(syc->print_rpt7_n,&buf[offset],sizeof(syc->print_rpt7_n)-1);
        offset += sizeof(syc->print_rpt7_n)-1;
        stint(syc->keep_data_for_n_days,&buf[offset]);
        offset += INTSIZE;
	stchar(&syc->send_mail, &buf[offset], sizeof(syc->send_mail));
	offset += sizeof(syc->send_mail);
	stchar(syc->system_mgr, &buf[offset], sizeof(syc->system_mgr)-1);
	offset += sizeof(syc->system_mgr);
	stint(syc->keep_cust_days,&buf[offset]);
	offset += INTSIZE;


}


/* Load program variables from C-ISAM data record */
void
unfill_syc( req_buf, buf )
     char *req_buf;
     char *buf;
{
	struct  cisam_syc  *syc;
	register int  offset = 0;

	syc = (struct cisam_syc*)req_buf;

        syc->day_start = ldint(&buf[offset]);
        offset += INTSIZE;

        syc->day_end = ldint(&buf[offset]);
        offset += INTSIZE;

        syc->prod_start_time = ldint(&buf[offset]);
        offset += INTSIZE;

	offset += sizeof(syc->fleet0);
	offset += sizeof(syc->index0);
	ldchar(&buf[offset],sizeof(syc->gen_rpt0_n)-1,syc->gen_rpt0_n);
        offset += sizeof(syc->gen_rpt0_n)-1;
	ldchar(&buf[offset],sizeof(syc->print_rpt0_n)-1,syc->print_rpt0_n);
        offset += sizeof(syc->print_rpt0_n)-1;

	offset += sizeof(syc->fleet0);
	offset += sizeof(syc->index0);
	ldchar(&buf[offset],sizeof(syc->gen_rpt1_n)-1,syc->gen_rpt1_n);
        offset += sizeof(syc->gen_rpt1_n)-1;
	ldchar(&buf[offset],sizeof(syc->print_rpt1_n)-1,syc->print_rpt1_n);
        offset += sizeof(syc->print_rpt1_n)-1;

	offset += sizeof(syc->fleet0);
	offset += sizeof(syc->index0);
	ldchar(&buf[offset],sizeof(syc->gen_rpt2_n)-1,syc->gen_rpt2_n);
        offset += sizeof(syc->gen_rpt2_n)-1;
	ldchar(&buf[offset],sizeof(syc->print_rpt2_n)-1,syc->print_rpt2_n);
        offset += sizeof(syc->print_rpt2_n)-1;

	offset += sizeof(syc->fleet0);
	offset += sizeof(syc->index0);
	ldchar(&buf[offset],sizeof(syc->gen_rpt3_n)-1,syc->gen_rpt3_n);
        offset += sizeof(syc->gen_rpt3_n)-1;
	ldchar(&buf[offset],sizeof(syc->print_rpt3_n)-1,syc->print_rpt3_n);
        offset += sizeof(syc->print_rpt3_n)-1;

	offset += sizeof(syc->fleet0);
	offset += sizeof(syc->index0);
	ldchar(&buf[offset],sizeof(syc->gen_rpt4_n)-1,syc->gen_rpt4_n);
        offset += sizeof(syc->gen_rpt4_n)-1;
	ldchar(&buf[offset],sizeof(syc->print_rpt4_n)-1,syc->print_rpt4_n);
        offset += sizeof(syc->print_rpt4_n)-1;

	offset += sizeof(syc->fleet0);
	offset += sizeof(syc->index0);
	ldchar(&buf[offset],sizeof(syc->gen_rpt5_n)-1,syc->gen_rpt5_n);
        offset += sizeof(syc->gen_rpt5_n)-1;
	ldchar(&buf[offset],sizeof(syc->print_rpt5_n)-1,syc->print_rpt5_n);
        offset += sizeof(syc->print_rpt5_n)-1;

	offset += sizeof(syc->fleet0);
	offset += sizeof(syc->index0);
	ldchar(&buf[offset],sizeof(syc->gen_rpt6_n)-1,syc->gen_rpt6_n);
        offset += sizeof(syc->gen_rpt6_n)-1;
	ldchar(&buf[offset],sizeof(syc->print_rpt6_n)-1,syc->print_rpt6_n);
        offset += sizeof(syc->print_rpt6_n)-1;

	offset += sizeof(syc->fleet0);
	offset += sizeof(syc->index0);
	ldchar(&buf[offset],sizeof(syc->gen_rpt7_n)-1,syc->gen_rpt7_n);
        offset += sizeof(syc->gen_rpt7_n)-1;
	ldchar(&buf[offset],sizeof(syc->print_rpt7_n)-1,syc->print_rpt7_n);
        offset += sizeof(syc->print_rpt7_n)-1;
        syc->keep_data_for_n_days  = ldint(&buf[offset]);
        offset += INTSIZE;
	syc->send_mail = buf[offset];
	offset += sizeof(syc->send_mail);
	ldchar(&buf[offset], sizeof(syc->system_mgr)-1, syc->system_mgr);
	offset += sizeof(syc->system_mgr);
	syc->keep_cust_days = ldint(&buf[offset]);
	offset += INTSIZE;
}

