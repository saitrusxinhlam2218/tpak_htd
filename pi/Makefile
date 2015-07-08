
# /****************************************************************************
# *                                                                           *
# *                           S C C S   I N F O                               *
# *                                                                           *
# * @(#)  pi.mk; Rel: 10.1.2.2; Get date: 5/24/95 at 14:41:47
# * @(#)  Last delta: 5/24/95 at 14:41:38
# * @(#)  SCCS File: /taxi/sccs/s.pi.mk
# *                                                                           *
# *****************************************************************************/

include ../Makefile.inc

IN1=/usr/include
IN2=../include
IN3=../monitor
ENH=../enhance_lib
LIB=../lib
# INTFLAGS is passed as an argument to make
#CFLAGS =  -DHR24 -DSCANDINAVIA -I$(IN2) -I$(IN3) -g $(INTFLAGS) $(REL_FLAGS) -DNDEBUG $(LG)
#CFLAGS = -g -non_shared -I$(IN2) -I$(IN3) -I$(ENH) -DINTERNATIONAL -DSCANDINAVIA $(REL_FLAGS) -DNDEBUG
#LIBFLAGS = -L$(LIB) -L$(ENH) -lm -ldb -lisam -lfill -ldbenhance -lcommon -lcnapi

LIBFLAGS = -L../lib -L$(ENH) \
	-lm -ldb -lbsd -lfill -ldbenhance -lcommon -lm $(CISAMDB_LIBS)

OBJS = pi.o  pi_dt_tm.o pi_fill_u.o pi_act_init.o pi_action.o  pi_restrict_time_call.o \
			pi_zone.o pi_mesg.o pi_excepts.o pi_call_h.o pi_sem.o \
			pi_account_db.o pi_account_map.o \
			pi_account_time.o pi_unzoned.o pi_log.o 


OBJ2 = pi_test.o

.c.o:
	xlc $(CFLAGS) -c $<

pi:  $(OBJS)
	xlc $(CFLAGS) $(OBJS) $(LIBFLAGS) -o pi


pi_action.o 	: ../pi/pi.h ../pi/pi_struc.h $(IN2)/except.h $(IN2)/fleet.h
pi_action.o 	: $(IN2)/mads_types.h
pi_act_init.o	: $(IN2)/mad_ipc.h $(IN2)/mads_types.h $(IN2)/menu.h $(IN2)/user.h
pi_act_init.o	: $(IN2)/calls.h $(IN2)/except.h $(IN2)/ui_def.h
pi_act_init.o	: $(IN2)/key.h ../pi/pi.h $(IN2)/language.h
pi_fill_u.o	: $(IN2)/user.h $(IN2)/mads_types.h $(IN2)/mads_isam.h
pi_fill_u.o	: $(IN2)/except.h $(IN2)/mad_ipc.h $(IN2)/ui_def.h $(IN2)/language.h
pi_dt_tm.o 	: $(IN2)/language.h
pi_db.o 	: $(IN2)/mads_types.h
pi_db.o		: $(IN2)/mads_isam.h $(IN2)/db_prof.h
pi_mesg.o 	: $(IN2)/df_maint.h $(IN2)/ex_key.h $(IN2)/language.h
pi_mesg.o 	: $(IN2)/mad_ipc.h ../pi/pi.h
pi_mesg.o 	: $(IN2)/mads_isam.h $(IN2)/mads_types.h $(IN2)/mesg.h
pi_mesg.o 	: $(IN2)/path.h $(IN2)/ui_error.h $(IN2)/ui_ipc.h
pi_mesg.o 	: $(IN2)/ui_strdefs.h $(IN2)/user.h
pi_zone.o 	: $(IN2)/call_entry.h $(IN2)/df_maint.h $(IN2)/ex_key.h
pi_zone.o 	: $(IN2)/language.h 
pi_zone.o 	: $(IN2)/mads_isam.h $(IN2)/mads_types.h
pi_zone.o 	: $(IN2)/screens.h $(IN2)/ui_def.h $(IN2)/ui_strdefs.h
pi_zone.o : $(IN2)/zgeo.h
pi.o : $(IN2)/except.h
pi_excepts.o	: $(IN2)/mads_types.h $(IN2)/mads_isam.h $(IN2)/calls.h
pi_excepts.o	: $(IN2)/except.h $(IN2)/mad_ipc.h $(IN2)/df_maint.h
pi_excepts.o	: $(IN2)/call_entry.h $(IN2)/ui_error.h $(IN2)/ui_ipc.h
pi_excepts.o    : $(IN2)/writer.h $(IN2)/mesg.h $(IN2)/language.h
pi_excepts.o    : $(IN2)/semops.h
pi_sem.o        : $(IN2)/semops.h
