#***********************************************************************
#                 RCS INFO
#
#  @(#)  $RCSfile: Makefile,v $
#  @(#)  $Revision: 1.4 $
#  @(#)  $Date: 2002/04/04 19:51:04 $
#  @(#)  $Author: jwelch $
#  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/auto_call/Makefile,v $
#
#  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
#
#**********************************************************************/
#ident "@(#) head:$RCSfile: Makefile,v $	$Revision: 1.4 $"

include ../Makefile.inc

INC =	../include
LIB =	../lib
DEST =	../../../usr/taxi/bin
ENH=../enhance_lib

#CFLAGS=	-g -non_shared -I$(INC) -DINTERNATIONAL -DDECLARING $(REL_FLAGS)

SRCS =	ac_main.c ac_generic_rqst.c ac_generic_replies.c ac_zone_addr.o \
	ac_trace.c

OBJS =	ac_main.o ac_generic_rqst.o ac_generic_replies.o ac_zone_addr.o \
	ac_trace.o

PGM =	autocall

LIBFLAGS = -L../lib -L$(ENH) -lTES -lacad -lindv \
	-lm -ldb -lbsd -lfill -lcommon -lm $(CISAMDB_LIBS)

LIBS = -lTES -lacad -lindv \
	-lm -ldb -lisam -lots -lbsd -lfill -lrate -lcommon 

.c.o:
	xlc $(CFLAGS) -c $<

all:	$(PGM)

clean:
	rm -f $(PGM) $(OBJS)

autocall:	$(OBJS)
	xlc $(CFLAGS) $(OBJS) $(LIBFLAGS) -o autocall
