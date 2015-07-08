#
#                 RCS INFO
#
#  @(#)  $RCSfile: Makefile,v $
#  @(#)  $Revision: 1.2 $
#  @(#)  $Date: 2002/03/29 14:53:44 $
#  @(#)  $Author: jwelch $
#  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/lib/SCRN/Makefile,v $
#
#  Copyright (c) 1991 - Motorola Mobile Data Division, Seattle, WA
#
#ident "@(#) head:$RCSfile: Makefile,v $	$Revision: 1.2 $"
#
include ../Makefile.inc
SRC =../..
INC=$(SRC)/include
#INCDIRS=-I$(INC)/$(LANGUAGE) -I$(INC)
#CFLAGS = -g -DINTERNATIONAL
#CC = cc -c $(INCDIRS)

SRCS =  Scrn.c \
	Scrn_read.c \
	Scrn_choice.c \
	Scrn_choice_alloc.c \
	Scrn_choice_free.c \
	Scrn_choice_window.c

OBJS=$(SRCS:.c=.o)

all: libscrn.a

clean:
	rm -f libscrn.a $(OBJS)

depend:
	makedep -m $(INCDIRS) -r $(SRCS)

libscrn.a : $(OBJS)
	ar ru libscrn.a $(OBJS) ;\
	ranlib libscrn.a ;\
	cp libscrn.a $(SRC)/lib
 
lint :
	lint -I../../include/g -o Scrn *.c

Scrn_choice.o : ../../include/Object.h ../../include/Scrn.h
Scrn_choice.o : ../../include/Scrn_private.h ../../include/taxipak.h
Scrn_choice_alloc.o : ../../include/Object.h ../../include/Scrn.h
Scrn_choice_alloc.o : ../../include/Scrn_private.h
Scrn_choice_alloc.o : ../../include/mem_mgmt.h ../../include/taxi_error.h
Scrn_choice_alloc.o : ../../include/taxipak.h
Scrn_choice_free.o : ../../include/Object.h ../../include/Scrn.h
Scrn_choice_free.o : ../../include/Scrn_private.h
Scrn_choice_free.o : ../../include/taxipak.h
Scrn_choice_window.o : ../../include/Object.h ../../include/Scrn.h
Scrn_choice_window.o : ../../include/Scrn_private.h
Scrn_choice_window.o : ../../include/func_keys.h ../../include/taxipak.h
Scrn.o : ../../include/Scrn.h ../../include/taxipak.h ../../include/tokenize.h
Scrn_read.o : ../../include/Scrn.h ../../include/taxipak.h
# DO NOT DELETE THIS LINE - makedep depends on it

Scrn.o : ../../include/Object.h ../../include/Object_defines.h
Scrn.o : ../../include/Object_functions.h ../../include/Object_types.h
Scrn.o : ../../include/STD.h ../../include/Scrn.h ../../include/mads_types.h
Scrn.o : ../../include/taxi_defs.h ../../include/taxi_error.h
Scrn.o : ../../include/taxipak.h ../../include/tokenize.h
Scrn_read.o : ../../include/Object.h ../../include/Object_defines.h
Scrn_read.o : ../../include/Object_functions.h ../../include/Object_types.h
Scrn_read.o : ../../include/STD.h ../../include/Scrn.h
Scrn_read.o : ../../include/mads_types.h ../../include/taxi_defs.h
Scrn_read.o : ../../include/taxi_error.h ../../include/taxipak.h
Scrn_choice.o : ../../include/Object.h ../../include/Object_defines.h
Scrn_choice.o : ../../include/Object_functions.h ../../include/Object_types.h
Scrn_choice.o : ../../include/STD.h ../../include/Scrn.h
Scrn_choice.o : ../../include/Scrn_private.h ../../include/mads_types.h
Scrn_choice.o : ../../include/taxi_defs.h ../../include/taxi_error.h
Scrn_choice.o : ../../include/taxipak.h ../../include/ui_strdefs.h
Scrn_choice_alloc.o : ../../include/Object.h ../../include/Object_defines.h
Scrn_choice_alloc.o : ../../include/Object_functions.h
Scrn_choice_alloc.o : ../../include/Object_types.h ../../include/STD.h
Scrn_choice_alloc.o : ../../include/Scrn.h ../../include/Scrn_private.h
Scrn_choice_alloc.o : ../../include/mads_types.h ../../include/mem_mgmt.h
Scrn_choice_alloc.o : ../../include/taxi_defs.h ../../include/taxi_error.h
Scrn_choice_alloc.o : ../../include/taxipak.h ../../include/ui_strdefs.h
Scrn_choice_free.o : ../../include/Object.h ../../include/Object_defines.h
Scrn_choice_free.o : ../../include/Object_functions.h
Scrn_choice_free.o : ../../include/Object_types.h ../../include/STD.h
Scrn_choice_free.o : ../../include/Scrn.h ../../include/Scrn_private.h
Scrn_choice_free.o : ../../include/mads_types.h ../../include/taxi_defs.h
Scrn_choice_free.o : ../../include/taxi_error.h ../../include/taxipak.h
Scrn_choice_free.o : ../../include/ui_strdefs.h
Scrn_choice_window.o : ../../include/Object.h ../../include/Object_defines.h
Scrn_choice_window.o : ../../include/Object_functions.h
Scrn_choice_window.o : ../../include/Object_types.h ../../include/STD.h
Scrn_choice_window.o : ../../include/Scrn.h ../../include/Scrn_private.h
Scrn_choice_window.o : ../../include/func_keys.h ../../include/mads_types.h
Scrn_choice_window.o : ../../include/taxi_defs.h ../../include/taxi_error.h
Scrn_choice_window.o : ../../include/taxipak.h ../../include/ui_strdefs.h

# DO NOT ADD ANYTHING HERE - it will disappear the next time makedep is run
