#/****************************************************************************
#*                                                                           *
#*                           S C C S   I N F O                               *
#*                                                                           *
#* @(#)  daily.mk; Rel: 10.1.0.0; Get date: 4/18/95 at 17:01:22
#* @(#)  Last delta: 12/2/94 at 11:35:25
#* @(#)  SCCS File: /taxi/sccs/s.daily.mk
#*                                                                           *
#* Copyright (c) 1990 - MDI Seattle                                          *
#*                                                                           *
#*****************************************************************************/
#ident "@(#) head:daily.mk	10.1.0.0"

IN1=/usr/include
IN2=../include
COM = ../common
DB=../db
LIB=../lib
# INTFLAGS is passed as an argument to make
CFLAGS= -non_shared -g -I$(IN2) -DHR24 -DINTERNATIONAL $(INTFLAGS)
#CFLAGS= -g -I$(IN2) -DHR12 -DINTERNATIONAL $(INTFLAGS)
LIBFLAGS = -L$(LIB) -lm -lisam -lfill -lrate

all:  da_report da_view da_purge da_subcall

OBJS = $(COM)/getoffsets.o $(COM)/error.o $(COM)/dt_tm.o getsyscntl.o date_time.o \
	map_dt_tm.o $(COM)/verify_dt.o $(COM)/toupper.o

PROCS = da_cabs.o da_cabs_hr.o da_calbak.o da_calls.o da_charge.o \
	da_dr_mess.o da_drivers.o da_emerge.o da_manual.o da_suspend.o \
	da_sys_mes.o da_unzoned.o da_usest.o da_vehic.o da_zones.o da_shift.o \
	da_credit.o da_autob.o

########################################
DA_VIEW_OBJ = da_view.o chosfleets.o $(DB)/db.o $(COM)/lang.o \
	$(COM)/local_date_time.o $(COM)/cls.o $(COM)/toupper.o
da_view: $(DA_VIEW_OBJ)
	cc $(DA_VIEW_OBJ) $(LIBFLAGS) -o da_view 

########################################
DA_PURGE_OBJ = da_purge.o printline.o chosfleets.o print_end.o report_h.o \
	$(COM)/local_date_time.o $(COM)/lang.o $(OBJS) $(DB)/db.o 
da_purge: $(DA_PURGE_OBJ)
	cc $(DA_PURGE_OBJ) $(LIBFLAGS) -o da_purge 

########################################
DA_REPORT_OBJ = da_report.o printline.o chosfleets.o getrpname.o print_end.o \
	report_h.o $(COM)/local_date_time.o $(COM)/lang.o $(OBJS) $(PROCS) $(DB)/db.o
da_report: $(DA_REPORT_OBJ)
	cc $(DA_REPORT_OBJ) $(LIBFLAGS) -o da_report 

########################################
DA_SUBCALL_OBJ = da_subcall.o $(COM)/verify_dt.o $(COM)/getoffsets.o $(COM)/error.o \
	$(COM)/dt_tm.o $(COM)/local_date_time.o $(COM)/lang.o $(DB)/db.o \
	$(COM)/toupper.o
da_subcall: $(DA_SUBCALL_OBJ)
	cc $(DA_SUBCALL_OBJ) $(LIBFLAGS) -o da_subcall

########################################

da_autob.o : ../include/df_maint.h ../include/ex_key.h ../include/language.h
da_autob.o : ../include/mads_isam.h ../include/mads_types.h ../include/path.h
da_autob.o : ../include/rep_strdefs.h
da_cabs.o : ../include/path.h ../include/rep_strdefs.h
da_cabs_hr.o : ../include/df_maint.h ../include/ex_key.h ../include/mads_isam.h
da_cabs_hr.o : ../include/mads_types.h ../include/path.h
da_cabs_hr.o : ../include/rep_strdefs.h
da_calbak.o : ../include/acc.h ../include/df_maint.h ../include/ex_key.h
da_calbak.o : ../include/mad_error.h ../include/mads_isam.h
da_calbak.o : ../include/mads_types.h ../include/path.h
da_calbak.o : ../include/rep_strdefs.h ../include/veh_driv.h
da_calbak.o : ../include/writer.h ../include/writer_strdefs.h
da_calls.o : ../include/df_maint.h ../include/ex_key.h ../include/language.h
da_calls.o : ../include/mads_isam.h ../include/mads_types.h ../include/path.h
da_calls.o : ../include/rep_strdefs.h
da_charge.o : ../include/df_maint.h ../include/ex_key.h ../include/mads_isam.h
da_charge.o : ../include/mads_types.h ../include/path.h
da_charge.o : ../include/rep_strdefs.h
da_credit.o : ../include/language.h ../include/path.h ../include/rep_strdefs.h
da_dr_mess.o : ../include/path.h ../include/rep_strdefs.h
da_drivers.o : ../include/path.h ../include/rep_strdefs.h
da_emerge.o : ../include/path.h ../include/rep_strdefs.h
da_manual.o : ../include/df_maint.h ../include/ex_key.h ../include/mads_isam.h
da_manual.o : ../include/mads_types.h ../include/path.h
da_manual.o : ../include/rep_strdefs.h
da_purge.o : ../include/comm_strdefs.h ../include/dr_maint.h ../include/key.h
da_purge.o : ../include/mad_error.h ../include/mad_ipc.h ../include/language.h
da_purge.o : ../include/mads_types.h ../include/path.h ../include/rep_strdefs.h
da_report.o : ../include/comm_strdefs.h ../include/key.h ../include/language.h
da_report.o : ../include/mad_error.h ../include/mads_types.h ../include/path.h
da_report.o : ../include/rep_strdefs.h ../include/writer_strdefs.h
da_shift.o : ../include/df_maint.h ../include/ex_key.h ../include/mad_error.h
da_shift.o : ../include/mads_isam.h ../include/mads_types.h ../include/path.h
da_shift.o : ../include/rep_strdefs.h
da_subcall.o : ../include/calls.h ../include/comm_strdefs.h ../include/except.h
da_subcall.o : ../include/key.h ../include/language.h ../include/literals.h
da_subcall.o : ../include/mad_error.h ../include/mad_ipc.h
da_subcall.o : ../include/mads_types.h ../include/path.h
da_subcall.o : ../include/rep_strdefs.h ../include/ui_ipc.h ../include/writer.h
da_subcall.o : ../include/writer_strdefs.h
da_suspend.o : ../include/path.h ../include/rep_strdefs.h
da_sys_mes.o : ../include/path.h ../include/rep_strdefs.h
da_unzoned.o : ../include/path.h ../include/rep_strdefs.h
da_usest.o : ../include/path.h ../include/rep_strdefs.h
da_usesum.o : ../include/key.h ../include/mad_ipc.h ../include/mads_types.h
da_usesum.o : ../include/path.h ../include/rep_strdefs.h
da_vehic.o : ../include/path.h ../include/rep_strdefs.h
da_view.o : ../include/key.h ../include/language.h ../include/mad_error.h
da_view.o : ../include/mads_types.h ../include/path.h ../include/rep_strdefs.h
da_zones.o : ../include/df_maint.h ../include/ex_key.h ../include/language.h
da_zones.o : ../include/mads_isam.h ../include/mads_types.h ../include/path.h
da_zones.o : ../include/rep_strdefs.h
date_time.o : ../include/rep_strdefs.h
getsyscntl.o : ../include/df_maint.h ../include/ex_key.h ../include/mads_isam.h
getsyscntl.o : ../include/mads_types.h ../include/rep_strdefs.h
date_time.o : ../include/rep_strdefs.h
map_dt_tm.o : ../include/rep_strdefs.h
chosfleets.o : ../include/df_maint.h ../include/ex_key.h ../include/language.h
chosfleets.o : ../include/mad_error.h ../include/mads_isam.h
chosfleets.o : ../include/mads_types.h ../include/rep_strdefs.h
print_end.o : ../include/rep_strdefs.h
report_h.o : ../include/rep_strdefs.h
printline.o : ../include/rep_strdefs.h
getrpname.o : ../include/path.h ../include/rep_strdefs.h
