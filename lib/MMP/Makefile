#/***********************************************************************
#*                 RCS INFO
#*
#*  @(#)  $RCSfile: Makefile,v $
#*  @(#)  $Revision: 2.1 $
#*  @(#)  $Date: 2015/07/21 09:21:43 $
#*  @(#)  $Author: jazam $
#*  @(#)  $Source: /usr/users/jwelch/cvs_g7/taxi-g7/lib/MMP/Makefile,v $
#*
#*  Copyright (c) 1991 - Motorola Mobile Data Division, Seattle, WA
#*
#***********************************************************************/
include ../Makefile.inc

SRCS =   mdt_init.c out_msg.c msg_mmp.c mdt_4.c pkt_id.c

OBJS = $(SRCS:.c=.o)

SUP_FLAGS = -DDUAL_PORT

all: libmmp.a

clean:
	rm -f libmmp.a $(OBJS)

libmmp.a : $(OBJS)
	ar ru libmmp.a $(OBJS) ;\
	ranlib libmmp.a ;
	cp  libmmp.a .. ;

lint :
	lint -I../../include/g -o fill *.c

depend:
	makedep -m -I$(IN2) -r $(SRCS)

