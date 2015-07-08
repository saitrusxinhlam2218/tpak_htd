#***********************************************************************
#                 RCS INFO
#
#  @(#)  $RCSfile: Makefile,v $
#  @(#)  $Revision: 1.6 $
#  @(#)  $Date: 2004/05/12 12:20:05 $
#  @(#)  $Author: jwelch $
#  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/lib/List/Makefile,v $
#
#  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
#
#**********************************************************************/
#ident "@(#) head:$RCSfile: Makefile,v $	$Revision: 1.6 $"

#static char rcsid[] = "$Id: Makefile,v 1.6 2004/05/12 12:20:05 jwelch Exp $";
include ../Makefile.inc
SRC=../..
INC=$(SRC)/include
#INCDIRS=-I$(INC)/$(LANGUAGE) -I$(INC)
LIB = ..
#CFLAGS = $(INCDIRS) -DINTERNATIONAL -g $(INTFLAGS) $(REL_FLAGS)

SRCS = 	LL.c \
	Fleet_lists.c \
	Call_lists.c \
	Zone_lists.c

OBJS = $(SRCS:.c=.o)

all: libList.a

clean:
	rm -f libList.a $(OBJS)

depend:
	makedep -m $(INCDIRS) -r $(SRCS)


libList.a: $(OBJS)
	ar ru libList.a $(OBJS) ;\
	ranlib libList.a ;\
	cp libList.a $(SRC)/lib


