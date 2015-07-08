#/****************************************************************************
#*                                                                           *
#*                           S C C S   I N F O                               *
#*                                                                           *
#* @(#)  audit.mk; Rel: 10.1.0.0; Get date: 4/18/95 at 16:59:51
#* @(#)  Last delta: 12/2/94 at 11:27:09
#* @(#)  SCCS File: /taxi/sccs/s.audit.mk
#*                                                                           *
#* Copyright (c) 1990 - MDI Seattle                                          *
#*                                                                           *
#*****************************************************************************/
#ident "@(#) head:audit.mk	10.1.0.0"

IN1=/usr/include
IN2=../include
COM=../common
DB=../db
LIB=../lib
# INTFLAGS is passed as an argument to make
CFLAGS = -non_shared -g -I$(IN2) -DHR24 -DINTERNATIONAL $(INTFLAGS)
#CFLAGS = -g -I$(IN2) -DHR12 -DINTERNATIONAL $(INTFLAGS)
LIBFLAGS = -L$(LIB) -lm -ldb -lisam -lfill -lrate

all: au_live au_message au_trail au_spawn

########################################
AU_LIVE_OBJ = au_live.o $(COM)/dt_tm.o date_time.o map_dt_tm.o $(COM)/verify_dt.o \
	$(COM)/local_date_time.o $(COM)/lang.o $(COM)/toupper.o
au_live: $(AU_LIVE_OBJ)
	cc $(CFLAGS) $(AU_LIVE_OBJ) $(LIBFLAGS) -o au_live

########################################
AU_MESSAGE_OBJ = au_message.o  $(COM)/lang.o $(COM)/local_date_time.o $(COM)/toupper.o
au_message: $(AU_MESSAGE_OBJ)
	cc $(CFLAGS) $(AU_MESSAGE_OBJ) $(LIBFLAGS) -o au_message

########################################
AU_TRAIL_OBJ = au_trail.o printline.o $(COM)/com_misc.o $(COM)/lang.o $(COM)/local_date_time.o \
	$(COM)/toupper.o
au_trail: $(AU_TRAIL_OBJ)
	cc $(CFLAGS) $(AU_TRAIL_OBJ) $(LIBFLAGS) -o au_trail

########################################
AU_SPAWN_OBJ =au_spawn.o $(COM)/lang.o
au_spawn: $(AU_SPAWN_OBJ)
	cc $(CFLAGS) $(AU_SPAWN_OBJ) $(LIBFLAGS) -o au_spawn

########################################

au_live.o : ../include/comm_strdefs.h ../include/key.h ../include/language.h
au_live.o : ../include/mads_types.h ../include/path.h ../include/rep_strdefs.h
au_live.o : ../include/ui_def.h
au_message.o : ../include/key.h ../include/language.h ../include/mads_types.h
au_message.o : ../include/path.h ../include/rep_strdefs.h ../include/ui_def.h
au_spawn.o : ../include/path.h
au_trail.o : ../include/key.h ../include/language.h ../include/mads_types.h
au_trail.o : ../include/path.h ../include/rep_strdefs.h
date_time.o : ../include/rep_strdefs.h
map_dt_tm.o : ../include/rep_strdefs.h
printline.o : ../include/rep_strdefs.h
