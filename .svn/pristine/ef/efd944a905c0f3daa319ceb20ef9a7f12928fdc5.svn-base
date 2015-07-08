#***********************************************************************
#                 RCS INFO
#
#  @(#)  $RCSfile: Makefile,v $
#  @(#)  $Revision: 1.6 $
#  @(#)  $Date: 2001/11/21 19:26:26 $
#  @(#)  $Author: taxiadm $
#  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/lib/Call/Makefile,v $
#
#  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
#
#**********************************************************************/
#ident "@(#) head:$RCSfile: Makefile,v $	$Revision: 1.6 $"

#static char rcsid[] = "$Id: Makefile,v 1.6 2001/11/21 19:26:26 taxiadm Exp $";

include ../Makefile.inc
SRC=../..
INC=../../include
#INCDIRS=-I$(INC)
ENH=../../enhance_lib


LIB = ..
#CFLAGS = $(INCDIRS) -I$(ENH) -DHR24 -DINTERNATIONAL -g $(INTFLAGS) $(REL_FLAGS)

SRCS = 	Call_info.c \
	Call_rec_info.c \
	Call_history.c \
	Call_util.c

OBJS = $(SRCS:.c=.o)

all: libCall.a

clean:
	rm -f libCall.a $(OBJS)


depend:
	makedep -m $(INCDIRS) -r $(SRCS)



libCall.a: $(OBJS)
	ar ru libCall.a $(OBJS) ;\
	ranlib libCall.a ;\
	cp libCall.a $(SRC)/lib


# DO NOT DELETE THIS LINE - makedep depends on it

