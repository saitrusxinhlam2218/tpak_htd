#***********************************************************************
#                 RCS INFO
#
#  @(#)  $RCSfile: Makefile,v $
#  @(#)  $Revision: 1.2 $
#  @(#)  $Date: 1996/09/06 13:55:49 $
#  @(#)  $Author: jwelch $
#  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/CN_lib/Makefile,v $
#
#  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
#
#**********************************************************************/
#ident "@(#) head:$RCSfile: Makefile,v $	$Revision: 1.2 $"

#static char rcsid[] = "$Id: Makefile,v 1.2 1996/09/06 13:55:49 jwelch Exp $";

SRC =..
IN1 = /usr/include
IN2 = ../include
CFLAGS = -g -I$(IN2)
OBJS =	libtcp.o cnapi.o

libcnapi.a : $(OBJS)
	ar ru libcnapi.a $(OBJS) ;\
	ranlib libcnapi.a ;\
	cp libcnapi.a $(SRC)/lib
