#/***********************************************************************
#*                 RCS INFO
#*
#*  @(#)  $RCSfile: Makefile,v $
#*  @(#)  $Revision: 1.2 $
#*  @(#)  $Date: 1998/07/23 16:57:29 $
#*  @(#)  $Author: taxiadm $
#*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/lib/INDV/Makefile,v $
#*
#*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
#*
#***********************************************************************/
#ident "@(#) head:$RCSfile: Makefile,v $	$Revision: 1.2 $"
#
#static char rcsid[] = "$Id: Makefile,v 1.2 1998/07/23 16:57:29 taxiadm Exp $";

include ../Makefile.inc
MK_FNAME=-f Makefile
TARGET=libindv.a
EXE=indv

SET_TESTING=-DTEST_INDIVIDUAL # turn on main() function creation

ARCH=$(MACHINE)

SRC=../..
INC=../../include
#INCDIRS=-I$(INC)/$(LANGUAGE) -I$(INC)
LIBS=-ldb -lcommon -lfill -lisam
LIBDIRS=-L$(SRC)/lib -L/sea/taxi-g7/src/lib -L/usr/lib
LIBFLAGS=$(LIBDIRS) $(LIBS)

#CFLAGS = $(INTFLAGS) -g $(INCDIRS) -D$(ARCH) $(I_AM_TESTING)

SRCS =	INDV_maint.c \
	INDV.c \
	INDV_list.c \
	INDV_choice.c 

OBJS = $(SRCS:.c=.o)

all:	$(TARGET)

$(TARGET):	$(OBJS)
		rm -f $(EXE) ;\
		ar ru $(TARGET) $(OBJS) ;\
		ranlib $(TARGET) ;\
		cp $(TARGET) $(SRC)/lib ;\
		echo \($(TARGET)\) now up-to-date.

clean:
	rm -f $(TARGET) $(EXE) $(OBJS)

depend:
	makedep -m $(INCDIRS) -r $(SRCS)


new:	$(TARGET)
lib:	$(TARGET)

$(EXE):
		rm -f $(OBJS) $(EXE) ; \
		make _exe I_AM_TESTING="$(SET_TESTING)" $(MK_FNAME) -$(MAKEFLAGS); \
		rm -f $(OBJS) ; \
		echo "" ;\
		echo Makefile rm\'s *.o :: preventing TEST .o\'s from getting into $(TARGET). ;\
		echo "" ;

_exe:	$(OBJS)
		cc $(OBJS) $(LIBFLAGS) -o $(EXE)

exe:	$(EXE)

