#/****************************************************************************
#*                                                                           *
#*                           S C C S   I N F O                               *
#*                                                                           *
#* @(#)  demand.mk; Rel: 10.1.0.0; Get date: 4/18/95 at 17:02:04
#* @(#)  Last delta: 12/2/94 at 11:15:03
#* @(#)  SCCS File: /taxi/sccs/s.demand.mk
#*                                                                           *
#* Copyright (c) 1990 - MDI Seattle                                          *
#*                                                                           *
#*****************************************************************************/
#ident "@(#) head:demand.mk	10.1.0.0"

IN1=/usr/include
IN2=../include
COM=../common
DB=../db
LIB=../lib
OBJ=chosfleets.o 
# INTFLAGS is passed as an argument to make
#CFLAGS= -g -I$(IN2) -DHR12 -DINTERNATIONAL $(INTFLAGS) -li
CFLAGS = -non_shared -g -I$(IN2) -DHR24 -DINTERNATIONAL $(INTFLAGS) 
LIBFLAGS = -L$(LIB) -lm -ldb -lisam -lfill -lrate

all: de_drive de_autob de_live de_subsc de_unasig de_vehic de_actveh de_umain de_lmain \
     de_mvehic de_manifst

########################################
DE_DRIVE_OBJ = de_drive.o $(COM)/lang.o chosfleets.o $(COM)/toupper.o
de_drive: $(DE_DRIVE_OBJ)
	cc $(CFLAGS) $(DE_DRIVE_OBJ) $(LIBFLAGS) -o de_drive

########################################
DE_AUTOB_OBJ = de_autob.o $(COM)/lang.o chosfleets.o $(COM)/toupper.o
de_autob: $(DE_AUTOB_OBJ)
	cc $(CFLAGS) $(DE_AUTOB_OBJ) $(LIBFLAGS) -o de_autob

########################################
DE_LIVE_OBJ = de_live.o chosfleets.o date_time.o map_dt_tm.o choosedttm.o $(COM)/lang.o \
		$(COM)/local_date_time.o $(COM)/verify_dt.o $(COM)/dt_tm.o $(COM)/toupper.o
de_live: $(DE_LIVE_OBJ)
	cc $(CFLAGS) $(DE_LIVE_OBJ) $(LIBFLAGS) -o de_live

########################################
DE_LMAIN_OBJ = de_lmain.o chosfleets.o date_time.o map_dt_tm.o $(COM)/verify_dt.o \
	choosedttm.o $(COM)/error.o printline.o $(COM)/dt_tm.o  \
	$(COM)/lang.o $(COM)/local_date_time.o  $(COM)/toupper.o $(COM)/com_misc.o
de_lmain: $(DE_LMAIN_OBJ)
	cc $(CFLAGS) $(DE_LMAIN_OBJ) $(LIBFLAGS) -o de_lmain

########################################
DE_SUBSC_OBJ = de_subsc.o chosfleets.o $(COM)/lang.o $(COM)/local_date_time.o \
	$(COM)/toupper.o
de_subsc: $(DE_SUBSC_OBJ)
	cc $(CFLAGS) $(DE_SUBSC_OBJ) $(LIBFLAGS) -o de_subsc

########################################
DE_UNASIG_OBJ = de_unasig.o chosfleets.o date_time.o map_dt_tm.o $(COM)/verify_dt.o \
	choosedttm.o $(COM)/dt_tm.o $(COM)/lang.o $(COM)/local_date_time.o   \
	 $(COM)/toupper.o
de_unasig: $(DE_UNASIG_OBJ)
	cc $(CFLAGS) $(DE_UNASIG_OBJ) $(LIBFLAGS) -o de_unasig

########################################
DE_UMAIN_OBJ = de_umain.o chosfleets.o date_time.o map_dt_tm.o $(COM)/verify_dt.o \
	choosedttm.o $(COM)/error.o printline.o $(COM)/dt_tm.o  \
	$(COM)/lang.o $(COM)/local_date_time.o $(COM)/toupper.o $(COM)/com_misc.o
de_umain: $(DE_UMAIN_OBJ)
	cc $(CFLAGS) $(DE_UMAIN_OBJ) $(LIBFLAGS) -o de_umain

########################################
DE_VEHIC_OBJ = de_vehic.o chosfleets.o $(COM)/verify_dt.o choosedttm.o date_time.o \
	$(COM)/dt_tm.o  $(COM)/lang.o $(COM)/local_date_time.o  $(COM)/toupper.o
de_vehic: $(DE_VEHIC_OBJ)
	cc $(CFLAGS) $(DE_VEHIC_OBJ) $(LIBFLAGS) -o de_vehic

########################################
DE_ACTVEH_OBJ = de_actveh.o $(COM)/lang.o $(COM)/local_date_time.o printline.o 
de_actveh: $(DE_ACTVEH_OBJ)
	cc $(CFLAGS) $(DE_ACTVEH_OBJ) $(LIBFLAGS) -o de_actveh

########################################
DE_MVEHIC_OBJ = de_mvehic.o chosfleets.o date_time.o $(COM)/verify_dt.o $(COM)/dt_tm.o \
	 $(COM)/lang.o $(COM)/local_date_time.o $(COM)/toupper.o
de_mvehic: $(DE_MVEHIC_OBJ)
	cc $(CFLAGS) $(DE_MVEHIC_OBJ) $(LIBFLAGS) -o de_mvehic

########################################
DE_MANIFST_OBJ = de_manifst.o printline.o $(COM)/lang.o $(COM)/local_date_time.o 
de_manifst: $(DE_MANIFST_OBJ)
	cc $(CFLAGS) $(DE_MANIFST_OBJ) $(LIBFLAGS) -o de_manifst

########################################

de_actveh.o : ../include/key.h ../include/mads_types.h ../include/rep_strdefs.h
de_actveh.o : ../include/srch_const.h
de_autob.o : ../include/key.h ../include/machdep.h ../include/mads_types.h
de_autob.o : ../include/path.h ../include/rep_strdefs.h
de_drive.o : ../include/key.h ../include/machdep.h ../include/mads_types.h
de_drive.o : ../include/path.h ../include/rep_strdefs.h
de_live.o : ../include/comm_strdefs.h ../include/ex_key.h ../include/key.h
de_live.o : ../include/machdep.h ../include/mad_error.h ../include/mads_types.h
de_live.o : ../include/path.h ../include/rep_strdefs.h ../include/ui_def.h
de_lmain.o : ../include/comm_strdefs.h ../include/ex_key.h ../include/key.h
de_lmain.o : ../include/language.h ../include/mad_error.h
de_lmain.o : ../include/mads_types.h ../include/rep_strdefs.h
de_manifst.o : ../include/key.h ../include/mads_types.h
de_manifst.o : ../include/rep_strdefs.h ../include/srch_const.h
de_mvehic.o : ../include/comm_strdefs.h ../include/key.h ../include/language.h
de_mvehic.o : ../include/machdep.h ../include/mads_types.h ../include/path.h
de_mvehic.o : ../include/rep_strdefs.h ../include/ui_def.h
de_subsc.o : ../include/key.h ../include/machdep.h ../include/mads_types.h
de_subsc.o : ../include/path.h ../include/rep_strdefs.h
de_umain.o : ../include/comm_strdefs.h ../include/ex_key.h ../include/key.h
de_umain.o : ../include/language.h ../include/mad_error.h
de_umain.o : ../include/mads_types.h ../include/rep_strdefs.h
de_unasig.o : ../include/comm_strdefs.h ../include/key.h ../include/machdep.h
de_unasig.o : ../include/mads_types.h ../include/path.h
de_unasig.o : ../include/rep_strdefs.h ../include/ui_def.h
de_vehic.o : ../include/comm_strdefs.h ../include/key.h ../include/language.h
de_vehic.o : ../include/machdep.h ../include/mads_types.h ../include/path.h
de_vehic.o : ../include/rep_strdefs.h ../include/ui_def.h
chosfleets.o : ../include/df_maint.h ../include/ex_key.h ../include/language.h
chosfleets.o : ../include/mad_error.h ../include/mads_isam.h
chosfleets.o : ../include/mads_types.h ../include/rep_strdefs.h
date_time.o : ../include/rep_strdefs.h
map_dt_tm.o : ../include/rep_strdefs.h
printline.o : ../include/rep_strdefs.h
choosedttm.o : ../include/rep_strdefs.h
