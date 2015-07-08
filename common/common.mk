#****************************************************************************
#*                                                                           *
#*                           S C C S   I N F O                               *
#*                                                                           *
#* @(#)  common.mk; Rel: 10.1.2.2; Get date: 5/24/95 at 14:45:29
#* @(#)  Last delta: 5/24/95 at 14:45:21
#* @(#)  SCCS File: /taxi/sccs/s.common.mk
#*                                                                           *
#*****************************************************************************/
#ident "@(#) head:common.mk	10.1.2.2"

IN1=/usr/include
IN2=../include
LIB = ../lib
CFLAGS = -I$(IN2) -DHR24 -DINTERNATIONAL -g $(INTFLAGS)

all : usprintf.o error.o sem_oper.o dt_tm.o verify_dt.o getoffsets.o getactflts.o set_dates.o ui_fld_atr.o \
	maint_atr.o super_lead.o com_misc.o open_catalog.o get.o getkey.o scanterm.o cls.o lang.o lock_credit.o \
	toupper.o local_date_time.o m_catgets.o

usprintf.o : usprintf.c

error.o : error.c  \
	 $(IN1)/stdio.h $(IN1)/sys/types.h $(IN2)/mads_types.h $(IN2)/mads_isam.h $(IN1)/isam.h $(IN2)/fleet.h $(IN2)/mad_error.h \
	 $(IN1)/stdio.h $(IN2)/df_maint.h $(IN2)/ex_key.h $(IN1)/isam.h


sem_oper.o : sem_oper.c  \
	 $(IN1)/sys/types.h $(IN1)/fcntl.h $(IN1)/sys/ipc.h $(IN1)/sys/sem.h $(IN2)/semops.h


dt_tm.o : dt_tm.c  \
	 $(IN1)/stdio.h $(IN1)/time.h $(IN1)/sys/types.h $(IN2)/language.h


verify_dt.o : verify_dt.c  \
	 $(IN1)/stdio.h $(IN1)/string.h $(IN1)/ctype.h $(IN1)/time.h $(IN2)/call_entry.h $(IN2)/language.h $(IN2)/comm_strdefs.h


getoffsets.o : getoffsets.c  \
	 $(IN1)/sys/types.h $(IN1)/sys/ipc.h $(IN1)/sys/shm.h $(IN1)/errno.h $(IN2)/mad_error.h $(IN1)/stdio.h $(IN2)/mad_ipc.h


getactflts.o : getactflts.c  \
	 $(IN1)/stdio.h $(IN1)/string.h $(IN1)/sys/types.h $(IN2)/mads_types.h $(IN2)/mad_error.h $(IN1)/stdio.h \
	 $(IN2)/ex_key.h $(IN1)/isam.h $(IN2)/df_maint.h $(IN2)/mads_isam.h $(IN1)/isam.h $(IN2)/fleet.h


set_dates.o : set_dates.c  \
	 $(IN1)/time.h

ui_fld_atr.o : ui_fld_atr.c  \
	 $(IN1)/curses.h $(IN2)/call_entry.h

maint_atr.o : maint_atr.c  \
	 $(IN1)/curses.h $(IN2)/fl_scr.h

super_lead.o : super_lead.c  \
	 $(IN1)/stdio.h $(IN1)/time.h $(IN1)/sys/types.h $(IN2)/mads_types.h $(IN2)/mad_error.h $(IN1)/stdio.h $(IN2)/mad_ipc.h \
	 $(IN2)/fleet.h

open_catalog.o : open_catalog.c \
	$(IN1)/stdio.h $(IN1)/nl_types.h $(IN2)/comm_strdefs.h

get.o : get.c \
	$(IN1)/cursesX.h $(IN1)/strings.h $(IN1)/sys/types.h $(IN1)/sys/timeb.h

getkey.o : getkey.c \
	$(IN1)/cursesX.h $(IN2)/tokenize.h

scanterm.o : scanterm.c \
	$(IN1)/cursesX.h $(IN1)/strings.h $(IN2)/tokenize.h $(IN2)/comm_strdefs.h

com_misc.o : com_misc.c

lang.o : lang.c \
	$(IN1)/nl_types.h $(IN1)/locale.h $(IN2)/path.h

lock_credit.o: ../include/path.h ../include/credit.h

local_date_time.o : local_date_time.c

m_catgets.o : m_catgets.c \
	$(IN1)/nl_types.h
