#/****************************************************************************
#*                                                                           *
#*                           S C C S   I N F O                               *
#*                                                                           *
#* @(#)  ui.mk; Rel: 10.1.2.2; Get date: 5/24/95 at 14:50:01
#* @(#)  Last delta: 5/24/95 at 14:49:52
#* @(#)  SCCS File: /taxi/sccs/s.ui.mk
#*                                                                           *
#* Copyright (c) 1990 - MDI Seattle                                          *
#*                                                                           *
#*****************************************************************************/
#ident "@(#) head:ui.mk	10.1.2.2"

include ../Makefile.inc
IN1=/usr/include
IN2=../include
CM = ../common
MN = ../monitor
ENH=../enhance_lib



.c.o:
	xlc $(CFLAGS) -c $<

# INTFLAGS is passed as an argument to make

#CFLAGS =  -g -non_shared -DHR24 -DINTERNATIONAL -DNDEBUG -I$(IN2) -I$(ENH) -g -DTRACE_FLAT -DSCANDINAVIA $(REL_FLAGS) $(LG)


OBJ = ui_main.o ui_init.o ui_error.o ui_menu.o ui_process.o ui_securty.o ui_disp_m.o ui_cleanup.o ui_misc.o ui_fill_u.o \
      ui_prt_err.o ui_exec_p.o ui_exec_shift_s.o ui_bas_scr.o ui_disp_s.o read_field.o check_fld.o ui_prt_def.o ui_ftnkey.o zone_addr.o \
      ui_excepts.o ui_zstat.o ui_sstat.o dsp_cl_dtl.o sp_ck_fld.o sp_disp.o sp_scr.o ui_call_h.o set_bit.o help.o \
      mesg_funcs.o ui_mesg.o kdt_scr.o kdt_disp.o kdt_ck_fld.o vhdr_cl.o cl_search.o dsp_cl_sum.o \
	  ck_ln_no.o unassign.o cl_vd_srch.o cl_as_srch.o crit_vhdr.o all_cl.o crit_all.o dp_al_scrn.o crit_sf.o sbsc_ft_cl.o \
	  cl_vh_srch.o assign.o crit_asgn.o crit_un.o cl_al_srch.o cl_in_srch.o indiv_cl.o cl_un_srch.o vh_dr_scrn.o vh_cl.o \
	  vh_dr_no.o dp_vh_dtl.o crit_vh.o dsp_vh_sum.o dp_dr_dtl.o dsp_dr_sum.o dr_cl.o crit_dr.o cl_dr_srch.o ck_usln_no.o \
	  cl_us_srch.o crit_us.o dp_us_dtl.o dp_us_scrn.o dsp_us_sum.o us_cl.o in_l_kys.o ui_altrn.o ui_unzoned.o ui_uud.o \
	  hist.o rst_sv_cls.o detail_key.o ui_subs.o ui_stat.o cl_sf_srch.o ui_resp.o ui_syserr.o mul_key.o \
		ui_restrict_time_calls.o ui_emerg.o sp_ui_misc.o ui_hist_cmt.o  \
	attr_select.o cl_zn_srch.o crit_zn.o zn_cl.o ui_acad.o ui_indv.o ui_emerg_bcst.o 

LIBFLAGS = -L../lib -L$(ENH) -lscrn -lcurses -ldb -lfill -ldbenhance -lcommon -lFleet -lCall -lm $(CISAMDB_LIBS)

ui:  $(OBJ)
	xlc $(CFLAGS) $(OBJ) $(LIBFLAGS) -o ui

$(MN)/newui.o : $(MN)/newui.c
	cc -c $(CFLAGS) $(MN)/newui.c -o $(MN)/newui.o


all_cl.o : ../include/call_entry.h ../include/df_maint.h ../include/func_keys.h
all_cl.o : ../include/language.h ../include/mads_isam.h ../include/mads_types.h
all_cl.o : ../include/screens.h ../include/srch.h ../include/srch_const.h
all_cl.o : ../include/ui_def.h ../include/ui_error.h
assign.o : ../include/call_entry.h ../include/df_maint.h ../include/func_keys.h
assign.o : ../include/language.h ../include/mads_isam.h ../include/mads_types.h
assign.o : ../include/screens.h ../include/srch.h ../include/srch_const.h
assign.o : ../include/ui_def.h ../include/ui_error.h ../include/writer.h
check_fld.o : ../include/call_entry.h ../include/calls.h ../include/df_maint.h
check_fld.o : ../include/func_keys.h ../include/language.h
check_fld.o : ../include/mads_isam.h ../include/mads_types.h
check_fld.o : ../include/screens.h ../include/ui_def.h ../include/ui_error.h
check_fld.o : ../include/ui_ipc.h ../include/ui_strdefs.h ../include/user.h
check_fld.o : ../include/zgeo.h
ck_ln_no.o : ../include/call_entry.h ../include/df_maint.h ../include/except.h
ck_ln_no.o : ../include/func_keys.h ../include/language.h
ck_ln_no.o : ../include/mads_isam.h ../include/mads_types.h
ck_ln_no.o : ../include/screens.h ../include/srch.h ../include/ui_def.h
ck_ln_no.o : ../include/ui_error.h ../include/ui_ipc.h ../include/ui_strdefs.h
ck_ln_no.o : ../include/user.h ../include/writer.h
ck_usln_no.o : ../include/call_entry.h ../include/df_maint.h
ck_usln_no.o : ../include/func_keys.h ../include/language.h
ck_usln_no.o : ../include/mads_isam.h ../include/mads_types.h
ck_usln_no.o : ../include/screens.h ../include/srch.h ../include/ui_def.h
ck_usln_no.o : ../include/ui_error.h ../include/ui_strdefs.h
cl_al_srch.o : ../include/call_entry.h ../include/df_maint.h
cl_al_srch.o : ../include/language.h ../include/mads_isam.h
cl_al_srch.o : ../include/mads_types.h ../include/srch.h
cl_al_srch.o : ../include/srch_const.h ../include/ui_def.h
cl_al_srch.o : ../include/ui_error.h
cl_as_srch.o : ../include/call_entry.h ../include/df_maint.h
cl_as_srch.o : ../include/language.h ../include/mads_isam.h
cl_as_srch.o : ../include/mads_types.h ../include/srch.h ../include/ui_def.h
cl_as_srch.o : ../include/ui_error.h
cl_dr_srch.o : ../include/call_entry.h ../include/df_maint.h
cl_dr_srch.o : ../include/language.h ../include/mads_isam.h
cl_dr_srch.o : ../include/mads_types.h ../include/srch.h ../include/ui_def.h
cl_dr_srch.o : ../include/ui_error.h
cl_in_srch.o : ../include/call_entry.h ../include/df_maint.h
cl_in_srch.o : ../include/language.h ../include/mads_isam.h
cl_in_srch.o : ../include/mads_types.h ../include/srch.h
cl_in_srch.o : ../include/srch_const.h ../include/ui_def.h
cl_in_srch.o : ../include/ui_error.h
cl_search.o : ../include/call_entry.h ../include/df_maint.h
cl_search.o : ../include/language.h ../include/mads_isam.h
cl_search.o : ../include/mads_types.h ../include/srch.h ../include/ui_strdefs.h
cl_sf_srch.o : ../include/call_entry.h ../include/df_maint.h
cl_sf_srch.o : ../include/language.h ../include/mads_isam.h
cl_sf_srch.o : ../include/mads_types.h ../include/srch.h
cl_sf_srch.o : ../include/srch_const.h ../include/ui_def.h
cl_sf_srch.o : ../include/ui_error.h
cl_un_srch.o : ../include/call_entry.h ../include/df_maint.h
cl_un_srch.o : ../include/language.h ../include/mads_isam.h
cl_un_srch.o : ../include/mads_types.h ../include/srch.h ../include/ui_def.h
cl_un_srch.o : ../include/ui_error.h
cl_us_srch.o : ../include/call_entry.h ../include/df_maint.h
cl_us_srch.o : ../include/language.h ../include/mads_isam.h
cl_us_srch.o : ../include/mads_types.h ../include/srch.h ../include/ui_def.h
cl_us_srch.o : ../include/ui_error.h
cl_vd_srch.o : ../include/call_entry.h ../include/df_maint.h
cl_vd_srch.o : ../include/language.h ../include/mads_isam.h
cl_vd_srch.o : ../include/mads_types.h ../include/srch.h ../include/ui_def.h
cl_vd_srch.o : ../include/ui_error.h
cl_vh_srch.o : ../include/call_entry.h ../include/df_maint.h
cl_vh_srch.o : ../include/language.h ../include/mads_isam.h
cl_vh_srch.o : ../include/mads_types.h ../include/srch.h ../include/ui_def.h
cl_vh_srch.o : ../include/ui_error.h
crit_all.o : ../include/call_entry.h ../include/df_maint.h
crit_all.o : ../include/func_keys.h ../include/mads_isam.h
crit_all.o : ../include/mads_types.h ../include/screens.h ../include/srch.h
crit_all.o : ../include/srch_const.h ../include/ui_def.h ../include/ui_error.h
crit_all.o : ../include/ui_strdefs.h ../include/user.h
crit_asgn.o : ../include/call_entry.h ../include/df_maint.h
crit_asgn.o : ../include/func_keys.h ../include/mads_isam.h
crit_asgn.o : ../include/mads_types.h ../include/screens.h ../include/srch.h
crit_asgn.o : ../include/srch_const.h ../include/ui_def.h ../include/ui_error.h
crit_asgn.o : ../include/ui_strdefs.h ../include/user.h /usr/include/time.h
crit_dr.o : ../include/call_entry.h ../include/df_maint.h
crit_dr.o : ../include/func_keys.h ../include/mads_isam.h
crit_dr.o : ../include/mads_types.h ../include/screens.h ../include/srch.h
crit_dr.o : ../include/srch_const.h ../include/ui_def.h ../include/ui_error.h
crit_dr.o : ../include/ui_strdefs.h ../include/user.h /usr/include/time.h
crit_sf.o : ../include/call_entry.h ../include/df_maint.h
crit_sf.o : ../include/func_keys.h ../include/mads_isam.h
crit_sf.o : ../include/mads_types.h ../include/screens.h ../include/srch.h
crit_sf.o : ../include/srch_const.h ../include/ui_def.h ../include/ui_error.h
crit_sf.o : ../include/ui_strdefs.h ../include/user.h /usr/include/time.h
crit_un.o : ../include/call_entry.h ../include/df_maint.h
crit_un.o : ../include/func_keys.h ../include/mads_isam.h
crit_un.o : ../include/mads_types.h ../include/screens.h ../include/srch.h
crit_un.o : ../include/srch_const.h ../include/ui_def.h ../include/ui_error.h
crit_un.o : ../include/ui_strdefs.h /usr/include/time.h
crit_us.o : ../include/call_entry.h ../include/df_maint.h
crit_us.o : ../include/func_keys.h ../include/mads_isam.h
crit_us.o : ../include/mads_types.h ../include/screens.h ../include/srch.h
crit_us.o : ../include/srch_const.h ../include/ui_def.h ../include/ui_strdefs.h
crit_us.o : ../include/user.h /usr/include/time.h
crit_vh.o : ../include/call_entry.h ../include/df_maint.h
crit_vh.o : ../include/func_keys.h ../include/mads_isam.h
crit_vh.o : ../include/mads_types.h ../include/screens.h ../include/srch.h
crit_vh.o : ../include/srch_const.h ../include/ui_def.h ../include/ui_error.h
crit_vh.o : ../include/ui_strdefs.h ../include/user.h /usr/include/time.h
crit_vhdr.o : ../include/call_entry.h ../include/df_maint.h
crit_vhdr.o : ../include/func_keys.h ../include/mads_isam.h
crit_vhdr.o : ../include/mads_types.h ../include/screens.h ../include/srch.h
crit_vhdr.o : ../include/srch_const.h ../include/ui_def.h ../include/ui_error.h
crit_vhdr.o : ../include/ui_strdefs.h ../include/user.h
detail_key.o : ../include/call_entry.h ../include/df_maint.h
detail_key.o : ../include/ex_key.h ../include/except.h ../include/func_keys.h
detail_key.o : ../include/language.h ../include/mads_isam.h
detail_key.o : ../include/mads_types.h ../include/menu.h ../include/screens.h
detail_key.o : ../include/srch_const.h ../include/ui_def.h
detail_key.o : ../include/ui_error.h ../include/ui_ipc.h
detail_key.o : ../include/ui_strdefs.h ../include/user.h ../include/writer.h
detail_key.o : ../include/zgeo.h
dp_al_scrn.o : ../include/menu.h ../include/screens.h ../include/ui_def.h
dp_al_scrn.o : ../include/ui_strdefs.h
dp_dr_dtl.o : ../include/call_entry.h ../include/df_maint.h
dp_dr_dtl.o : ../include/language.h ../include/mads_isam.h
dp_dr_dtl.o : ../include/mads_types.h ../include/ui_def.h /usr/include/time.h
dp_us_dtl.o : ../include/call_entry.h ../include/df_maint.h
dp_us_dtl.o : ../include/language.h ../include/mads_isam.h
dp_us_dtl.o : ../include/mads_types.h /usr/include/time.h
dp_us_scrn.o : ../include/menu.h ../include/screens.h ../include/ui_def.h
dp_us_scrn.o : ../include/ui_strdefs.h
dp_vh_dtl.o : ../include/call_entry.h ../include/df_maint.h
dp_vh_dtl.o : ../include/mads_isam.h ../include/mads_types.h
dp_vh_dtl.o : ../include/ui_def.h /usr/include/time.h
dr_cl.o : ../include/call_entry.h ../include/df_maint.h ../include/func_keys.h
dr_cl.o : ../include/language.h ../include/mads_isam.h ../include/mads_types.h
dr_cl.o : ../include/screens.h ../include/srch.h ../include/srch_const.h
dr_cl.o : ../include/ui_def.h ../include/ui_error.h
dsp_cl_dtl.o : ../include/call_entry.h ../include/df_maint.h
dsp_cl_dtl.o : ../include/language.h ../include/mads_isam.h
dsp_cl_dtl.o : ../include/mads_types.h ../include/screens.h ../include/ui_def.h
dsp_cl_dtl.o : ../include/ui_strdefs.h
dsp_cl_sum.o : ../include/call_entry.h ../include/df_maint.h
dsp_cl_sum.o : ../include/mads_isam.h ../include/mads_types.h
dsp_cl_sum.o : ../include/screens.h
dsp_dr_sum.o : ../include/call_entry.h ../include/df_maint.h
dsp_dr_sum.o : ../include/language.h ../include/mads_isam.h
dsp_dr_sum.o : ../include/mads_types.h ../include/srch.h ../include/ui_def.h
dsp_dr_sum.o : /usr/include/time.h
dsp_us_sum.o : ../include/call_entry.h ../include/df_maint.h
dsp_us_sum.o : ../include/language.h ../include/mads_isam.h
dsp_us_sum.o : ../include/mads_types.h ../include/srch.h ../include/ui_def.h
dsp_vh_sum.o : ../include/call_entry.h ../include/df_maint.h
dsp_vh_sum.o : ../include/language.h ../include/mads_isam.h
dsp_vh_sum.o : ../include/mads_types.h ../include/srch.h ../include/ui_def.h
help.o : ../include/call_entry.h ../include/df_maint.h ../include/func_keys.h
help.o : ../include/mads_isam.h ../include/mads_types.h ../include/screens.h
help.o : ../include/ui_def.h ../include/ui_error.h ../include/ui_ipc.h
help.o : ../include/ui_strdefs.h ../include/user.h
hist.o : ../include/call_entry.h ../include/df_maint.h ../include/func_keys.h
hist.o : ../include/mads_isam.h ../include/mads_types.h ../include/screens.h
hist.o : ../include/ui_def.h ../include/ui_error.h ../include/ui_ipc.h
hist.o : ../include/user.h
in_l_kys.o : ../include/call_entry.h ../include/func_keys.h
in_l_kys.o : ../include/language.h ../include/mads_isam.h
in_l_kys.o : ../include/mads_types.h ../include/screens.h ../include/srch.h
in_l_kys.o : ../include/ui_def.h ../include/ui_error.h ../include/zgeo.h
indiv_cl.o : ../include/call_entry.h ../include/func_keys.h
indiv_cl.o : ../include/language.h ../include/mads_isam.h
indiv_cl.o : ../include/mads_types.h ../include/screens.h ../include/srch.h
indiv_cl.o : ../include/ui_def.h ../include/ui_error.h
kdt_ck_fld.o : ../include/df_maint.h ../include/ex_key.h ../include/func_keys.h
kdt_ck_fld.o : ../include/kdt_ary.h ../include/language.h
kdt_ck_fld.o : ../include/mad_error.h ../include/mads_isam.h
kdt_ck_fld.o : ../include/mads_types.h ../include/path.h ../include/screens.h
kdt_ck_fld.o : ../include/ui_def.h ../include/ui_error.h ../include/ui_ipc.h
kdt_ck_fld.o : ../include/ui_strdefs.h ../include/user.h
kdt_disp.o : ../include/fleet.h ../include/kdt_ary.h ../include/mads_types.h
kdt_disp.o : ../include/menu.h ../include/screens.h ../include/ui_def.h
kdt_disp.o : ../include/ui_error.h ../include/ui_strdefs.h ../include/user.h
kdt_scr.o : ../include/df_maint.h ../include/func_keys.h ../include/kdt_ary.h
kdt_scr.o : ../include/kdt_scr_fd.h ../include/mads_isam.h
kdt_scr.o : ../include/mads_types.h ../include/screens.h ../include/ui_def.h
kdt_scr.o : ../include/ui_error.h ../include/ui_ipc.h ../include/user.h
mesg_funcs.o : ../include/df_maint.h ../include/ex_key.h ../include/language.h
mesg_funcs.o : ../include/mad_error.h ../include/mad_ipc.h
mesg_funcs.o : ../include/mads_isam.h ../include/mads_types.h ../include/mesg.h
mesg_funcs.o : ../include/path.h ../include/ui_error.h ../include/ui_ipc.h
mesg_funcs.o : ../include/ui_strdefs.h ../include/user.h
mul_key.o : ../include/df_maint.h ../include/ex_key.h ../include/except.h
mul_key.o : ../include/language.h ../include/mad_error.h ../include/mads_isam.h
mul_key.o : ../include/mads_types.h ../include/path.h ../include/screens.h
mul_key.o : ../include/srch.h ../include/ui_error.h ../include/ui_strdefs.h
mul_key.o : ../include/user.h ../include/writer.h
read_field.o : ../include/call_entry.h ../include/func_keys.h
read_field.o : ../include/language.h ../include/mads_types.h
read_field.o : ../include/screens.h ../include/ui_def.h ../include/user.h
rst_sv_cls.o : ../include/mads_isam.h ../include/mads_types.h ../include/srch.h
rst_sv_cls.o : ../include/srch_const.h
sbsc_ft_cl.o : ../include/call_entry.h ../include/df_maint.h
sbsc_ft_cl.o : ../include/func_keys.h ../include/language.h
sbsc_ft_cl.o : ../include/mads_isam.h ../include/mads_types.h
sbsc_ft_cl.o : ../include/screens.h ../include/srch.h ../include/srch_const.h
sbsc_ft_cl.o : ../include/ui_def.h ../include/ui_error.h
set_bit.o : ../include/mads_isam.h ../include/mads_types.h
sp_ck_fld.o : ../include/df_maint.h ../include/func_keys.h
sp_ck_fld.o : ../include/language.h ../include/mad_ipc.h ../include/mads_isam.h
sp_ck_fld.o : ../include/mads_types.h ../include/screens.h ../include/sp_ary.h
sp_ck_fld.o : ../include/ui_def.h ../include/ui_error.h ../include/ui_ipc.h
sp_ck_fld.o : ../include/ui_strdefs.h ../include/user.h
sp_disp.o : ../include/fleet.h ../include/mads_types.h ../include/menu.h
sp_disp.o : ../include/screens.h ../include/sp_ary.h ../include/ui_def.h
sp_disp.o : ../include/ui_error.h ../include/ui_strdefs.h ../include/user.h
sp_scr.o : ../include/df_maint.h ../include/language.h ../include/mads_isam.h
sp_scr.o : ../include/mads_types.h ../include/screens.h ../include/sp_ary.h
sp_scr.o : ../include/sp_scr_fld.h ../include/ui_def.h ../include/ui_error.h
sp_scr.o : ../include/ui_ipc.h ../include/ui_strdefs.h ../include/user.h
sp_ui_misc.o : sp_ui_misc.c
sp_ui_misc.o : ../include/mads_types.h
sp_ui_misc.o : ../include/df_maint.h
sp_ui_misc.o : ../include/ex_key.h
sp_ui_misc.o : ../include/mads_isam.h
sp_ui_misc.o : ../include/path.h
sp_ui_misc.o : ../include/except.h
sp_ui_misc.o : ../include/ui_strdefs.h
ui_altrn.o : ../include/call_entry.h ../include/df_maint.h
ui_altrn.o : ../include/func_keys.h ../include/language.h
ui_altrn.o : ../include/mads_isam.h ../include/mads_types.h
ui_altrn.o : ../include/screens.h ../include/ui_def.h ../include/ui_error.h
ui_altrn.o : ../include/ui_strdefs.h ../include/user.h ../include/zgeo.h
ui_bas_scr.o : ../include/call_entry.h ../include/df_maint.h
ui_bas_scr.o : ../include/func_keys.h ../include/language.h
ui_bas_scr.o : ../include/mads_isam.h ../include/mads_types.h
ui_bas_scr.o : ../include/scr_flds.h ../include/screens.h ../include/ui_def.h
ui_bas_scr.o : ../include/ui_error.h ../include/ui_ipc.h
ui_bas_scr.o : ../include/ui_strdefs.h ../include/user.h
ui_call_h.o : ../include/df_maint.h ../include/except.h ../include/language.h
ui_call_h.o : ../include/mad_ipc.h ../include/mads_isam.h
ui_call_h.o : ../include/mads_types.h ../include/ui_error.h ../include/ui_ipc.h
ui_call_h.o : ../include/ui_strdefs.h ../include/writer.h
ui_cleanup.o : ../include/comm_strdefs.h ../include/df_maint.h
ui_cleanup.o : ../include/mads_isam.h ../include/mads_types.h
ui_cleanup.o : ../include/ui_error.h ../include/ui_strdefs.h ../include/user.h
ui_cleanup.o : ../include/writer_strdefs.h
ui_disp_m.o : ../include/language.h ../include/menu.h ../include/screens.h
ui_disp_m.o : ../include/ui_def.h ../include/ui_strdefs.h ../include/user.h
ui_disp_s.o : ../include/call_entry.h ../include/fleet.h ../include/language.h
ui_disp_s.o : ../include/mads_types.h ../include/menu.h ../include/screens.h
ui_disp_s.o : ../include/ui_def.h ../include/ui_error.h ../include/ui_strdefs.h
ui_disp_s.o : ../include/user.h
ui_emerg.o : ../include/except.h ../include/mad_ipc.h ../include/screens.h
ui_emerg.o : ../include/ui_strdefs.h
ui_error.o : ../include/ui_error.h ../include/ui_strdefs.h
ui_excepts.o : ../include/call_entry.h ../include/calls.h ../include/df_maint.h
ui_excepts.o : ../include/except.h ../include/func_keys.h ../include/language.h
ui_excepts.o : ../include/mad_ipc.h ../include/mads_isam.h
ui_excepts.o : ../include/mads_types.h ../include/menu.h ../include/mesg.h
ui_excepts.o : ../include/screens.h ../include/ui_def.h ../include/ui_error.h
ui_excepts.o : ../include/ui_ipc.h ../include/ui_strdefs.h ../include/user.h
ui_excepts.o : ../include/writer.h ../include/semops.h
ui_exec_p.o : ../include/df_maint.h ../include/language.h
ui_exec_p.o : ../include/mads_isam.h ../include/mads_types.h ../include/menu.h
ui_exec_p.o : ../include/ui_def.h ../include/ui_error.h ../include/ui_strdefs.h
ui_exec_shift_s.o : ../include/df_maint.h ../include/language.h
ui_exec_shift_s.o : ../include/mads_isam.h ../include/mads_types.h
ui_exec_shift_s.o : ../include/menu.h ../include/screens.h ../include/ui_def.h
ui_exec_shift_s.o : ../include/ui_error.h ../include/ui_strdefs.h
ui_fill_u.o : ../include/except.h ../include/language.h ../include/mad_ipc.h
ui_fill_u.o : ../include/mads_isam.h ../include/mads_types.h
ui_fill_u.o : ../include/ui_def.h ../include/user.h
ui_ftnkey.o : ../include/call_entry.h ../include/calls.h ../include/df_maint.h
ui_ftnkey.o : ../include/except.h ../include/func_keys.h ../include/language.h
ui_ftnkey.o : ../include/mads_isam.h ../include/mads_types.h ../include/path.h
ui_ftnkey.o : ../include/screens.h ../include/srch.h ../include/ui_def.h
ui_ftnkey.o : ../include/ui_error.h ../include/ui_ipc.h ../include/ui_strdefs.h
ui_ftnkey.o : ../include/user.h ../include/writer.h ../include/zgeo.h
ui_ftnkey.o : ../include/mad_ipc.h
ui_init.o : ../include/calls.h ../include/except.h ../include/key.h
ui_init.o : ../include/language.h ../include/mad_error.h ../include/mad_ipc.h
ui_init.o : ../include/mads_types.h ../include/menu.h ../include/screens.h
ui_init.o : ../include/ui_def.h ../include/ui_error.h ../include/ui_strdefs.h
ui_init.o : ../include/user.h
ui_main.o : ../include/comm_strdefs.h ../include/ui_def.h
ui_main.o : ../include/ui_strdefs.h ../include/writer_strdefs.h
ui_menu.o : ../include/calls.h ../include/language.h ../include/mads_isam.h
ui_menu.o : ../include/mads_types.h ../include/menu.h ../include/screens.h
ui_menu.o : ../include/srch.h ../include/ui_def.h ../include/ui_error.h
ui_menu.o : ../include/ui_ipc.h
ui_mesg.o : ../include/call_entry.h ../include/df_maint.h ../include/ex_key.h
ui_mesg.o : ../include/func_keys.h ../include/language.h ../include/mad_error.h
ui_mesg.o : ../include/mads_isam.h ../include/mads_types.h ../include/menu.h
ui_mesg.o : ../include/mesg.h ../include/path.h ../include/screens.h
ui_mesg.o : ../include/srch.h ../include/ui_def.h ../include/ui_error.h
ui_mesg.o : ../include/ui_strdefs.h ../include/user.h
ui_misc.o : ../include/call_entry.h ../include/calls.h ../include/df_maint.h
ui_misc.o : ../include/except.h ../include/language.h ../include/mads_isam.h
ui_misc.o : ../include/mads_types.h ../include/screens.h ../include/ui_def.h
ui_misc.o : ../include/ui_error.h ../include/ui_ipc.h ../include/ui_strdefs.h
ui_misc.o : ../include/user.h ../include/writer.h ../include/zgeo.h
ui_process.o : ../include/language.h ../include/screens.h ../include/tokenize.h
ui_process.o : ../include/ui_def.h ../include/ui_error.h
ui_process.o : ../include/ui_strdefs.h
ui_prt_def.o : ../include/call_entry.h ../include/calls.h ../include/language.h
ui_prt_def.o : ../include/mads_isam.h ../include/mads_types.h
ui_prt_def.o : ../include/screens.h ../include/ui_def.h ../include/ui_error.h
ui_prt_def.o : ../include/ui_ipc.h ../include/ui_strdefs.h ../include/user.h
ui_prt_def.o : ../include/writer.h ../include/zgeo.h
ui_prt_err.o : ../include/ui_error.h
ui_resp.o : ../include/path.h ../include/ui_def.h ../include/ui_error.h
ui_resp.o : ../include/ui_strdefs.h
ui_restrict_time_calls.o : ../include/call_entry.h ../include/df_maint.h
ui_restrict_time_calls.o : ../include/func_keys.h ../include/mads_isam.h
ui_restrict_time_calls.o : ../include/mads_types.h ../include/path.h
ui_restrict_time_calls.o : ../include/screens.h ../include/ui_def.h
ui_restrict_time_calls.o : ../include/ui_error.h
ui_restrict_time_calls.o : ../include/ui_restrict_time_calls.h
ui_restrict_time_calls.o : ../include/ui_strdefs.h ../include/user.h
ui_securty.o : ../include/language.h ../include/menu.h ../include/screens.h
ui_securty.o : ../include/ui_def.h ../include/ui_error.h ../include/user.h
ui_sstat.o : ../include/call_entry.h ../include/df_maint.h ../include/ex_key.h
ui_sstat.o : ../include/func_keys.h ../include/language.h
ui_sstat.o : ../include/mad_error.h ../include/mads_isam.h
ui_sstat.o : ../include/mads_types.h ../include/menu.h ../include/screens.h
ui_sstat.o : ../include/ui_def.h ../include/ui_error.h ../include/ui_ipc.h
ui_sstat.o : ../include/ui_stat.h ../include/ui_strdefs.h ../include/user.h
ui_stat.o : ../include/calls.h ../include/df_maint.h ../include/except.h
ui_stat.o : ../include/language.h ../include/mad_ipc.h ../include/mads_isam.h
ui_stat.o : ../include/mads_types.h ../include/ui_def.h ../include/ui_error.h
ui_stat.o : ../include/ui_ipc.h ../include/ui_strdefs.h
ui_subs.o : ../include/call_entry.h ../include/df_maint.h ../include/language.h
ui_subs.o : ../include/mads_isam.h ../include/mads_types.h ../include/screens.h
ui_subs.o : ../include/srch.h ../include/ui_def.h ../include/ui_error.h
ui_subs.o : ../include/ui_ipc.h ../include/ui_strdefs.h ../include/user.h
ui_subs.o : ../include/writer.h ../include/zgeo.h
ui_syserr.o : ../include/ui_strdefs.h ../include/except.h
ui_unzoned.o : ../include/df_maint.h ../include/mads_isam.h
ui_unzoned.o : ../include/mads_types.h ../include/ui_error.h
ui_unzoned.o : ../include/ui_strdefs.h
ui_uud.o : ../include/call_entry.h ../include/calls.h ../include/df_maint.h
ui_uud.o : ../include/except.h ../include/func_keys.h ../include/language.h
ui_uud.o : ../include/mad_ipc.h ../include/mads_isam.h ../include/mads_types.h
ui_uud.o : ../include/menu.h ../include/screens.h ../include/ui_def.h
ui_uud.o : ../include/ui_error.h ../include/ui_strdefs.h ../include/ui_uud.h
ui_uud.o : ../include/user.h
ui_zstat.o : ../include/call_entry.h ../include/df_maint.h
ui_zstat.o : ../include/func_keys.h ../include/language.h
ui_zstat.o : ../include/mad_error.h ../include/mads_isam.h
ui_zstat.o : ../include/mads_types.h ../include/menu.h ../include/screens.h
ui_zstat.o : ../include/ui_def.h ../include/ui_error.h ../include/ui_ipc.h
ui_zstat.o : ../include/ui_stat.h ../include/ui_strdefs.h ../include/user.h
unassign.o : ../include/call_entry.h ../include/df_maint.h
unassign.o : ../include/func_keys.h ../include/language.h
unassign.o : ../include/mads_isam.h ../include/mads_types.h
unassign.o : ../include/screens.h ../include/srch.h ../include/srch_const.h
unassign.o : ../include/ui_def.h ../include/ui_error.h ../include/user.h
us_cl.o : ../include/call_entry.h ../include/df_maint.h ../include/func_keys.h
us_cl.o : ../include/language.h ../include/mads_isam.h ../include/mads_types.h
us_cl.o : ../include/screens.h ../include/srch.h ../include/ui_def.h
us_cl.o : ../include/ui_error.h ../include/ui_strdefs.h
vh_cl.o : ../include/call_entry.h ../include/df_maint.h ../include/func_keys.h
vh_cl.o : ../include/language.h ../include/mads_isam.h ../include/mads_types.h
vh_cl.o : ../include/screens.h ../include/srch.h ../include/srch_const.h
vh_cl.o : ../include/ui_def.h ../include/ui_error.h
vh_dr_no.o : ../include/call_entry.h ../include/df_maint.h
vh_dr_no.o : ../include/func_keys.h ../include/language.h
vh_dr_no.o : ../include/mads_isam.h ../include/mads_types.h
vh_dr_no.o : ../include/screens.h ../include/srch.h ../include/ui_def.h
vh_dr_no.o : ../include/ui_error.h ../include/ui_strdefs.h
vh_dr_scrn.o : ../include/menu.h ../include/screens.h ../include/ui_def.h
vh_dr_scrn.o : ../include/ui_strdefs.h
vhdr_cl.o : ../include/call_entry.h ../include/df_maint.h
vhdr_cl.o : ../include/func_keys.h ../include/language.h ../include/mads_isam.h
vhdr_cl.o : ../include/mads_types.h ../include/screens.h ../include/srch.h
vhdr_cl.o : ../include/srch_const.h ../include/ui_def.h ../include/ui_error.h
zone_addr.o : ../include/call_entry.h ../include/df_maint.h ../include/ex_key.h
zone_addr.o : ../include/language.h ../include/mad_error.h
zone_addr.o : ../include/mads_isam.h ../include/mads_types.h
zone_addr.o : ../include/screens.h ../include/ui_def.h ../include/ui_strdefs.h
zone_addr.o : ../include/zgeo.h

