static char sz__FILE__[]="@(#)fill_sn.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_sn.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:38:20
 * @(#)  Last delta: 12/2/94 at 11:32:34
 * @(#)  SCCS File: /taxi/sccs/s.fill_sn.c
 *                                                                           *
 *****************************************************************************/

#define KEY_SRC
#include "Street_db.h"
#include "taxi_db.h"

void
fill_sn_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
  struct cisam_sn *sn;
  register int  offset = 0;

  sn = (struct cisam_sn*)req_buf;

  stlong(sn->record_nbr,&buf[offset]);
  offset += LONGSIZE;

  stlong(sn->index,&buf[offset]);
  offset += LONGSIZE;

  stchar(&sn->rec_type,&buf[offset],sizeof(sn->rec_type));
  offset += sizeof(sn->rec_type); 

  stchar(sn->name,&buf[offset],sizeof(sn->name)-1);
  offset += sizeof(sn->name)-1; 

  stchar(sn->str_type,&buf[offset],sizeof(sn->str_type)-1);
}

void
fill_sn(req_buf, buf, action)
     char *req_buf;
     char *buf;
     int     action;
{
  struct cisam_sn *sn;
  register int  offset = 0;
  int serial_nbr = 0;

  sn = (struct cisam_sn*)req_buf;

  if (action == ADD_REC)
    {
      isuniqueid(Db_files[STRNAME_FILE_ID].fd,&serial_nbr);
      sn->record_nbr = (int)serial_nbr;
    }
  
  stlong(sn->record_nbr,&buf[offset]);
  offset += LONGSIZE;

  stlong(sn->index,&buf[offset]);
  offset += LONGSIZE;

  stchar(&sn->rec_type,&buf[offset],sizeof(sn->rec_type));
  offset += sizeof(sn->rec_type);

  stchar(sn->name,&buf[offset],sizeof(sn->name)-1);
  offset += sizeof(sn->name)-1; 

  stchar(sn->str_type,&buf[offset],sizeof(sn->str_type)-1);
}

void
unfill_sn(req_buf, buf)
     char *req_buf;
     char *buf;
{
  int  cc;
  struct cisam_sn *sn;
  register int  offset = 0;

  sn = (struct cisam_sn*)req_buf;

  sn->record_nbr = ldlong(&buf[offset]);
  offset += LONGSIZE;

  sn->index = ldlong(&buf[offset]);
  offset += LONGSIZE;

  sn->rec_type = buf[offset];
  offset += sizeof(sn->rec_type);

  ldchar(&buf[offset],sizeof(sn->name)-1,sn->name);
  offset += sizeof(sn->name)-1; 

  ldchar(&buf[offset],sizeof(sn->str_type)-1,sn->str_type);
}
