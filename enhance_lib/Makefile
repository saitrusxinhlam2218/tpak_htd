#***********************************************************************
#                 RCS INFO
#
#  @(#)  $RCSfile: Makefile,v $
#  @(#)  $Revision: 1.4 $
#  @(#)  $Date: 1998/06/18 17:35:10 $
#  @(#)  $Author: taxiadm $
#  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/enhance_lib/Makefile,v $
#
#  Copyright (c) 1994 - MobileSoft Consulting, Inc. Dublin, OH
#
#**********************************************************************/
#ident "@(#) head:$RCSfile: Makefile,v $	$Revision: 1.4 $"

include ../Makefile.inc

IN2 =../include

OBJS = taxi.o tcp.o system.o SemelMsg.o EnhSwitch.o

all:	 libenhance.a libdbenhance.a

libenhance.a:	$(OBJS) 
		ar ru libenhance.a $(OBJS) ;\
		ranlib libenhance.a

libdbenhance.a:	$(OBJS)
		ar ru libdbenhance.a $(OBJS) ; \
		ranlib libdbenhance.a


