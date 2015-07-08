#/***********************************************************************
#*                 RCS INFO
#*
#*  @(#)  $RCSfile: Makefile,v $
#*  @(#)  $Revision: 1.2 $
#*  @(#)  $Date: 1998/07/23 16:56:45 $
#*  @(#)  $Author: taxiadm $
#*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/lib/ACAD/Makefile,v $
#*
#*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
#*
#***********************************************************************/
#ident "@(#) head:$RCSfile: Makefile,v $	$Revision: 1.2 $"
#
#static char rcsid[] = "$Id: Makefile,v 1.2 1998/07/23 16:56:45 taxiadm Exp $";

include ../../Makefile.inc
MK_FNAME=-f Makefile
TARGET=libacad.a
EXE=acad

SET_TESTING=-DTEST_ACCOUNT_ADDRESS # turn on main() function creation

ARCH=$(MACHINE)

SRC=../..
INC=$(SRC)/include
INCDIRS=-I$(INC)/$(LANGUAGE) -I$(INC)
LIBS=-ldb -lcommon -lfill -lisam
LIBDIRS=-L$(SRC)/lib 
LIBFLAGS=$(LIBDIRS) $(LIBS)

#CFLAGS = $(INTFLAGS) -g $(INCDIRS) -D$(ARCH) $(I_AM_TESTING)

SRCS =	ACAD_maint.c ACAD.c 

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