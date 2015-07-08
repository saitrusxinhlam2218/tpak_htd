#***********************************************************************
#                 RCS INFO
#
#  @(#)  $RCSfile: Makefile,v $
#  @(#)  $Revision: 1.3 $
#  @(#)  $Date: 2002/03/25 17:49:34 $
#  @(#)  $Author: jwelch $
#  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/auto_tes/Makefile,v $
#
#  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
#
#**********************************************************************/
#ident "@(#) head:$RCSfile: Makefile,v $	$Revision: 1.3 $"
#
#static char rcsid[] = "$Id: Makefile,v 1.3 2002/03/25 17:49:34 jwelch Exp $";



INC =	../include
LIB =	../lib
DEST =	../../../usr/taxi/bin

CFLAGS=	-g -non_shared -I$(INC) -DINTERNATIONAL -DDECLARING -DTESTING $(REL_FLAGS)
LIBFLAGS =	

SRCS =	ac_main.c ac_generic_rqst.c ac_g7_rqst.c ac_g7_replies.c ac_generic_replies.c \
	ac_zone_addr.c ac_trace.c

OBJS =	ac_main.o ac_generic_rqst.o ac_g7_rqst.o ac_g7_replies.o ac_generic_replies.o \
	ac_zone_addr.o ac_trace.o

PGM =	auto_tes

LIBS = -lTES -lacad -lindv \
	-lm -ldb -lisam -lots -lbsd -lfill -lrate -lcommon 

SHLIBS = -ldbisam -lCall -lfill -lrate -lcommon -lm -lots -lbsd

all:	$(PGM)

clean:
	rm -f $(PGM) $(OBJS)

auto_tes:	$(OBJS)
	$(CC) -o $(PGM) $(CFLAGS) $(OBJS) -L$(LIB) $(LIBS)
