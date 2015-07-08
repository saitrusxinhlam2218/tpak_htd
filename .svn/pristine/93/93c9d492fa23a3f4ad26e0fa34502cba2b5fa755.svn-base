/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: fill_hia.c,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 1998/02/23 21:47:31 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill/fill_hia.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: fill_hia.c,v 1.3 1998/02/23 21:47:31 taxiadm Exp $";

#define KEY_SRC
#include "Channel_db.h"

void
fill_channel_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
  struct cisam_channel     *channel;
  register int offset = 0;

  channel = (struct cisam_channel *)req_buf;
  stchar(channel->channel_id, &buf[offset], sizeof(channel->channel_id));
}

void
fill_channel(req_buf, buf,action)
     char *req_buf;
     char *buf;
     int action;
{
  struct cisam_channel   *channel;
  register int offset = 0;

  channel = (struct cisam_channel *)req_buf;

  stchar(channel->channel_id, &buf[offset], sizeof(channel->channel_id));
  offset += sizeof(channel->channel_id);

  stlong(channel->channel_nbr, &buf[offset]);
  offset += sizeof(channel->channel_nbr);

  stchar(channel->channel_id, &buf[offset], sizeof(channel->channel_short_descript));
  offset += sizeof(channel->channel_short_descript);

  stchar(channel->channel_descript, &buf[offset], sizeof(channel->channel_descript));
  offset += sizeof(channel->channel_descript);

}

void
unfill_channel(req_buf, buf)
     char *req_buf;
     char *buf;
{
  struct cisam_channel   *channel;
  register int index = 0;

  channel = (struct cisam_channel *)req_buf;

  ldchar(&buf[index], sizeof(channel->channel_id), channel->channel_id);
  index += sizeof(channel->channel_id);

  channel->channel_nbr = ldlong(&buf[index]);
  index += sizeof(channel->channel_nbr);
  
  ldchar(&buf[index], sizeof(channel->channel_short_descript), channel->channel_short_descript);
  index += sizeof(channel->channel_short_descript);

  ldchar(&buf[index], sizeof(channel->channel_descript), channel->channel_descript);
  index += sizeof(channel->channel_descript);
}
