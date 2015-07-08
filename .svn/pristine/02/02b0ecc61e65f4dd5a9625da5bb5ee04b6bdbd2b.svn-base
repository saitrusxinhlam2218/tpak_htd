static char sz__FILE__[]="@(#)fill_imsi.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_imsi.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:38:37
 * @(#)  Last delta: 12/2/94 at 11:35:14
 * @(#)  SCCS File: /taxi/sccs/s.fill_imsi.c
 *                                                                           *
 *****************************************************************************/

#define KEY_SRC
#include "taxipak.h"
#include "Object.h"
#include "mads_types.h"
#include "IMSI_db.h"

void
fill_imsi_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_imsi  *imsi;

	imsi = (struct cisam_imsi*)req_buf;

	stchar(&imsi->fleet,&buf[VH_IMSI_FLEET],sizeof(imsi->fleet)); /* fleet indicator */

	stint(imsi->nbr, &buf[VH_IMSI_NBR]);  /* vehicle number */

	stchar(imsi->imsi, &buf[VH_IMSI_IMSI], sizeof(imsi->imsi)-1);
}

void
fill_imsi(req_buf, buf, action)
     char *req_buf;
     char *buf;
     int  action;
{
	struct  cisam_imsi  *imsi;
	register int  offset = 0;

	imsi = (struct cisam_imsi*)req_buf;

	stchar(&imsi->fleet,&buf[offset],sizeof(imsi->fleet));
	offset += sizeof(imsi->fleet);

	stint(imsi->nbr,&buf[offset]);
	offset += INTSIZE;

	stchar(imsi->imsi, &buf[offset], sizeof(imsi->imsi) -1);
	offset += sizeof(imsi->imsi) -1;
	
	stchar(imsi->telephone, &buf[offset], sizeof(imsi->telephone) -1);
	offset += sizeof(imsi->telephone) -1;	
}

/* Load program variables from C-ISAM data record */
void
unfill_imsi(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_imsi  *imsi;
	register int  offset = 0;
	char temp_attr[65];
	short code = 0;

	imsi = (struct cisam_imsi *)req_buf;

	imsi->fleet = buf[offset];
	offset += sizeof(imsi->fleet);

	imsi->nbr = ldint(&buf[offset]);
	offset += INTSIZE;

	ldchar(&buf[offset], sizeof(imsi->imsi) -1, imsi->imsi);
	offset += sizeof(imsi->imsi) -1;

	ldchar(&buf[offset], sizeof(imsi->telephone) -1, imsi->telephone);
	offset += sizeof(imsi->telephone) -1;	
	
}

