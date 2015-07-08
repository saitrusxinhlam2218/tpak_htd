#***********************************************************************
#                 RCS INFO
#
#  @(#)  $RCSfile: Makefile,v $
#  @(#)  $Revision: 1.5 $
#  @(#)  $Date: 2002/03/29 14:52:32 $
#  @(#)  $Author: jwelch $
#  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/lib/Fleet/Makefile,v $
#
#  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
#
#**********************************************************************/
#ident "@(#) head:$RCSfile: Makefile,v $	$Revision: 1.5 $"

#static char rcsid[] = "$Id: Makefile,v 1.5 2002/03/29 14:52:32 jwelch Exp $";

include ../Makefile.inc
SRC=../..
INC=$(SRC)/include
#INCDIRS=-I$(INC)/$(LANGUAGE) -I$(INC)
LIB = ..
#CFLAGS = $(INCDIRS) -DINTERNATIONAL -g $(INTFLAGS) $(REL_FLAGS)

SRCS = 	Fleet_info.c \
	Fleet_util.c

OBJS = $(SRCS:.c=.o)

all: libFleet.a

clean:
	rm -f libFleet.a $(OBJS)

depend:
	makedep -m $(INCDIRS) -r $(SRCS)


libFleet.a: $(OBJS)
	ar ru libFleet.a $(OBJS) ;\
	ranlib libFleet.a ;\
	cp libFleet.a $(SRC)/lib 

