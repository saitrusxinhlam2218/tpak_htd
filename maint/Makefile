#****************************************************************************
#*                                                                           *
#*                           S C C S   I N F O                               *
#*                                                                           *
#* @(#)  maint.mk; Rel: 6.4.0.0; Get date: 7/9/92 at 16:16:07
#* @(#)  Last delta: 3/8/91 at 19:03:36
#* @(#)  SCCS File: /taxi/sccs/s.maint.mk
#*                                                                           *
#*****************************************************************************/
#ident "@(#) head:maint.mk	6.4.0.0"

include ../Makefile.inc

LIBFLAGS = -L../lib -L../enhance_lib -ldbenhance -L../lib -ldb -lcommon -lcurses -lfill $(CISAMDB_LIBS)

all: dr_maint fl_maint geo_maint in_maint mf_maint up_maint vh_maint zn_maint res_per_maint hl_maint

#read_fld is used by most maint programs.
read_fld.o : ../include/call_entry.h ../include/mads_types.h
read_fld.o : ../include/ui_def.h ../include/user.h

#----------------------------------------------
# dr_maint

DR_OBJ = dr_ck_flds.o dr_cl_flds.o dr_common.o dr_maint.o read_fld.o \
	dr_sub1.o attr_select.o

dr_maint: $(DR_OBJ) 
	$(CC) -o dr_maint $(CFLAGS) $(DR_OBJ) $(LIBFLAGS)

dr_ck_flds.o : ../include/df_maint.h ../include/dr_m_strdefs.h
dr_ck_flds.o : ../include/dr_maint.h ../include/ex_key.h ../include/language.h
dr_ck_flds.o : ../include/literals.h ../include/mad_error.h
dr_ck_flds.o : ../include/mads_isam.h ../include/mads_types.h ../include/path.h
dr_ck_flds.o : ../include/tokenize.h
dr_cl_flds.o : ../include/dr_maint.h ../include/dr_scr_ary.h
dr_cl_flds.o : ../include/mad_error.h ../include/scr.h
dr_common.o : ../include/dr_m_strdefs.h ../include/dr_maint.h
dr_common.o : ../include/ex_key.h ../include/language.h ../include/literals.h
dr_common.o : ../include/mad_error.h ../include/mad_ipc.h
dr_common.o : ../include/mads_isam.h ../include/mads_types.h
dr_common.o : ../include/tokenize.h ../include/ui_ipc.h
dr_maint.o : ../include/comm_strdefs.h ../include/dr_m_strdefs.h
dr_maint.o : ../include/dr_maint.h ../include/key.h ../include/language.h
dr_maint.o : ../include/literals.h ../include/mad_error.h
dr_maint.o : ../include/mads_types.h
dr_sub1.o : ../include/df_maint.h ../include/dr_m_strdefs.h
dr_sub1.o : ../include/dr_maint.h ../include/ex_key.h ../include/language.h
dr_sub1.o : ../include/literals.h ../include/mad_error.h ../include/mad_ipc.h
dr_sub1.o : ../include/mads_isam.h ../include/mads_types.h ../include/path.h
dr_sub1.o : ../include/tokenize.h ../include/ui_ipc.h


#----------------------------------------------
# fl_maint

FL_OBJ = fl_maint.o fl_common.o fl_ck_flds.o fl_cl_flds.o fl_sub1.o fl_rd_fld.o

fl_maint: $(FL_OBJ)
	$(CC) -o fl_maint $(CFLAGS) $(FL_OBJ) $(LIBFLAGS) 


fl_ck_flds.o : ../include/df_maint.h ../include/ex_key.h
fl_ck_flds.o : ../include/fl_m_strdefs.h ../include/fl_maint.h
fl_ck_flds.o : ../include/fl_scr.h ../include/language.h ../include/literals.h
fl_ck_flds.o : ../include/mad_error.h ../include/mads_isam.h
fl_ck_flds.o : ../include/mads_types.h ../include/tokenize.h
fl_cl_flds.o : ../include/fl_maint.h ../include/fl_scr.h ../include/mad_error.h
fl_common.o : ../include/ex_key.h ../include/fl_m_strdefs.h
fl_common.o : ../include/fl_maint.h ../include/fl_scr.h ../include/fl_scr_ary.h
fl_common.o : ../include/language.h ../include/literals.h
fl_common.o : ../include/mad_error.h ../include/mad_ipc.h
fl_common.o : ../include/mads_isam.h ../include/mads_types.h
fl_common.o : ../include/tokenize.h ../include/ui_ipc.h
fl_maint.o : ../include/comm_strdefs.h ../include/fl_m_strdefs.h
fl_maint.o : ../include/fl_maint.h ../include/fl_scr.h ../include/key.h
fl_maint.o : ../include/language.h ../include/literals.h ../include/mad_error.h
fl_maint.o : ../include/mads_types.h
fl_rd_fld.o : ../include/fl_scr.h ../include/mads_types.h ../include/ui_def.h
fl_rd_fld.o : ../include/user.h
fl_sub1.o : ../include/df_maint.h ../include/ex_key.h ../include/fl_m_strdefs.h
fl_sub1.o : ../include/fl_maint.h ../include/fl_scr.h ../include/language.h
fl_sub1.o : ../include/literals.h ../include/mad_error.h ../include/mad_ipc.h
fl_sub1.o : ../include/mads_isam.h ../include/mads_types.h ../include/path.h
fl_sub1.o : ../include/tokenize.h ../include/ui_ipc.h


#----------------------------------------------
# geo_maint

GEO_OBJ = geo_maint.o  geo_ck_fld.o geo_cl_fld.o geo_sub1.o geo_sub2.o read_fld.o geo_sub3.o

geo_maint: $(GEO_OBJ) 
	cc -non_shared $(GEO_OBJ) $(LIBFLAGS) -o geo_maint

geo_ck_fld.o : ../include/df_maint.h ../include/ex_key.h
geo_ck_fld.o : ../include/geo_m_strdefs.h ../include/geo_msg.h
geo_ck_fld.o : ../include/geo_scr.h ../include/language.h
geo_ck_fld.o : ../include/mad_error.h ../include/mads_isam.h
geo_ck_fld.o : ../include/mads_types.h ../include/tokenize.h
geo_cl_fld.o : ../include/geo_ary.h ../include/geo_scr.h ../include/mad_error.h
geo_maint.o : ../include/comm_strdefs.h ../include/func_keys.h
geo_maint.o : ../include/geo_m_strdefs.h ../include/geo_msg.h
geo_maint.o : ../include/geo_scr.h ../include/key.h ../include/language.h
geo_maint.o : ../include/mad_error.h ../include/mad_ipc.h
geo_maint.o : ../include/mads_types.h ../include/path.h ../include/ui_def.h
geo_maint.o : ../include/ui_error.h ../include/user.h
geo_sub1.o : ../include/df_maint.h ../include/ex_key.h
geo_sub1.o : ../include/geo_m_strdefs.h ../include/geo_msg.h
geo_sub1.o : ../include/geo_scr.h ../include/language.h ../include/mad_error.h
geo_sub1.o : ../include/mads_isam.h ../include/mads_types.h
geo_sub1.o : ../include/tokenize.h
geo_sub2.o : ../include/df_maint.h ../include/ex_key.h
geo_sub2.o : ../include/geo_m_strdefs.h ../include/geo_msg.h
geo_sub2.o : ../include/geo_scr.h ../include/language.h ../include/mad_error.h
geo_sub2.o : ../include/mad_ipc.h ../include/mads_isam.h
geo_sub2.o : ../include/mads_types.h ../include/path.h ../include/tokenize.h
geo_sub2.o : ../include/ui_ipc.h
geo_sub3.o : ../include/df_maint.h ../include/ex_key.h
geo_sub3.o : ../include/geo_m_strdefs.h ../include/geo_msg.h
geo_sub3.o : ../include/geo_scr.h ../include/language.h ../include/mad_error.h
geo_sub3.o : ../include/mads_isam.h ../include/mads_types.h ../include/path.h
geo_sub3.o : ../include/tokenize.h


#----------------------------------------------
# in_maint

IN_OBJ = in_maint.o  in_cl_fld.o in_ck_fld.o  in_read_fld.o

in_maint: $(IN_OBJ) 
	cc -o in_maint $(CFLAGS) $(IN_OBJ) $(LIBFLAGS)

in_ck_fld.o : ../include/df_maint.h ../include/in_maint.h
in_ck_fld.o : ../include/mad_error.h ../include/mads_isam.h
in_ck_fld.o : ../include/mads_types.h ../include/in_m_strdefs.h
in_cl_fld.o : ../include/in_m_strdefs.h ../include/in_maint.h
in_maint.o : ../include/comm_strdefs.h ../include/in_m_strdefs.h
in_maint.o : ../include/in_maint.h ../include/in_msg.h ../include/key.h
in_maint.o : ../include/language.h ../include/mad_error.h
in_maint.o : ../include/mads_types.h ../include/path.h
in_maint.o : ../include/tokenize.h
in_read_fld.o : ../include/in_maint.h ../include/mads_types.h

#----------------------------------------------
# mf_maint


MF_OBJ = menu_maint.o  men_cl_fld.o men_ck_fld.o read_fld.o

mf_maint: $(MF_OBJ) 
	cc -o mf_maint $(CFLAGS) $(MF_OBJ) $(LIBFLAGS)

men_ck_fld.o : ../include/language.h ../include/mad_error.h
men_ck_fld.o : ../include/mads_isam.h ../include/mads_types.h
men_ck_fld.o : ../include/menu_m_strdefs.h ../include/menu_maint.h
men_ck_fld.o : ../include/tokenize.h
men_cl_fld.o : ../include/mad_error.h ../include/menscr_ary.h
men_cl_fld.o : ../include/menu_maint.h ../include/scr.h
menu_maint.o : ../include/comm_strdefs.h ../include/key.h ../include/language.h
menu_maint.o : ../include/mad_error.h ../include/mad_ipc.h
menu_maint.o : ../include/mads_types.h ../include/menu_m_strdefs.h
menu_maint.o : ../include/menu_maint.h ../include/tokenize.h
menu_maint.o : ../include/ui_ipc.h

#----------------------------------------------
# up_maint

UP_OBJ = up_maint.o  up_cl_flds.o up_ck_flds.o up_sub1.o up_sub2.o  read_fld.o

up_maint: $(UP_OBJ) 
	cc -o up_maint $(CFLAGS) $(UP_OBJ) $(LIBFLAGS)

up_ck_flds.o : ../include/df_maint.h ../include/ex_key.h ../include/language.h
up_ck_flds.o : ../include/literals.h ../include/mad_error.h
up_ck_flds.o : ../include/mads_isam.h ../include/mads_types.h ../include/path.h
up_ck_flds.o : ../include/tokenize.h ../include/up_m_strdefs.h
up_ck_flds.o : ../include/up_maint.h
up_cl_flds.o : ../include/mad_error.h ../include/scr.h
up_cl_flds.o : ../include/up_m_strdefs.h ../include/up_maint.h
up_cl_flds.o : ../include/up_scr_ary.h
up_maint.o : ../include/comm_strdefs.h ../include/key.h ../include/language.h
up_maint.o : ../include/literals.h ../include/mad_error.h
up_maint.o : ../include/mads_types.h ../include/menu.h ../include/path.h
up_maint.o : ../include/up_m_strdefs.h ../include/up_maint.h
up_maint.o : ../include/userpwd.h
up_sub1.o : ../include/df_maint.h ../include/ex_key.h ../include/language.h
up_sub1.o : ../include/literals.h ../include/mad_error.h ../include/mads_isam.h
up_sub1.o : ../include/mads_types.h ../include/path.h ../include/tokenize.h
up_sub1.o : ../include/up_m_strdefs.h ../include/up_maint.h
up_sub2.o : ../include/df_maint.h ../include/ex_key.h ../include/language.h
up_sub2.o : ../include/literals.h ../include/mad_error.h ../include/mad_ipc.h
up_sub2.o : ../include/mads_isam.h ../include/mads_types.h ../include/menu.h
up_sub2.o : ../include/path.h ../include/tokenize.h ../include/ui_ipc.h
up_sub2.o : ../include/up_m_strdefs.h ../include/up_maint.h

RP_OBJ = res_per_maint.o res_per_cl_fld.o res_per_ck_fld.o res_per_sub1.o res_per_common.o \
	res_per_read_field.o
RP_4GL_OBJ = res_per.o send_ipc.o globals.o messages.o array_mgmt.o 
RP_4GL_FORMS = res_per.frm

res_per_maint:	$(RP_4GL_OBJ) $(RP_4GL_FORMS)
	c4gl $(RP_4GL_OBJ) -o res_per_maint.4ge

.4gl.o:
	c4gl -c $<
.per.frm:
	form4gl $<

.SUFFIXES:
.SUFFIXES: .4gl .4go .4gi .ec .c .o .4ge .per .frm

HOL_OBJ = hol_maint.o hol_cl_fld.o hol_ck_fld.o hol_sub1.o hol_common.o hol_read_field.o
HOL_4GL_OBJ = hol.o send_ipc.o globals.o messages.o array_mgmt.o
HOL_4GL_FORMS = hol.frm
hl_maint: $(HOL_4GL_OBJ) $(HOL_4GL_FORMS)
	c4gl $(HOL_4GL_OBJ) -o hl_maint.4ge

#----------------------------------------------
# vh_maint

VH_OBJ = vh_maint.o  vh_cl_flds.o vh_ck_flds.o vh_sub1.o vh_common.o  read_fld.o attr_select.o

vh_maint: $(VH_OBJ) 
	cc -o vh_maint $(CFLAGS) $(VH_OBJ) $(LIBFLAGS)

vh_ck_flds.o : ../include/call_entry.h ../include/df_maint.h
vh_ck_flds.o : ../include/ex_key.h ../include/language.h ../include/literals.h
vh_ck_flds.o : ../include/mad_error.h ../include/mads_isam.h
vh_ck_flds.o : ../include/mads_types.h ../include/path.h ../include/tokenize.h
vh_ck_flds.o : ../include/vh_m_strdefs.h ../include/vh_maint.h
vh_cl_flds.o : ../include/mad_error.h ../include/scr.h
vh_cl_flds.o : ../include/vh_m_strdefs.h ../include/vh_maint.h
vh_cl_flds.o : ../include/vh_scr_ary.h
vh_common.o : ../include/ex_key.h ../include/language.h ../include/literals.h
vh_common.o : ../include/mad_error.h ../include/mad_ipc.h
vh_common.o : ../include/mads_isam.h ../include/mads_types.h
vh_common.o : ../include/tokenize.h ../include/ui_ipc.h
vh_common.o : ../include/vh_m_strdefs.h ../include/vh_maint.h
vh_maint.o : ../include/comm_strdefs.h ../include/key.h ../include/language.h
vh_maint.o : ../include/literals.h ../include/mad_error.h
vh_maint.o : ../include/mads_types.h ../include/vh_m_strdefs.h
vh_maint.o : ../include/vh_maint.h
vh_sub1.o : ../include/df_maint.h ../include/ex_key.h ../include/language.h
vh_sub1.o : ../include/literals.h ../include/mad_error.h ../include/mad_ipc.h
vh_sub1.o : ../include/mads_isam.h ../include/mads_types.h ../include/path.h
vh_sub1.o : ../include/tokenize.h ../include/ui_ipc.h ../include/vh_m_strdefs.h
vh_sub1.o : ../include/vh_maint.h

#----------------------------------------------
# zn_maint

ZN_OBJ = zn_maint.o zn_cl_fld.o zn_ck_fld.o zn_sub1.o zn_sub2.o read_fld.o

zn_maint: $(ZN_OBJ)
	cc -o zn_maint $(CFLAGS) $(ZN_OBJ) $(LIBFLAGS)

zn_ck_fld.o : ../include/df_maint.h ../include/ex_key.h ../include/language.h
zn_ck_fld.o : ../include/mad_error.h ../include/mads_isam.h
zn_ck_fld.o : ../include/mads_types.h ../include/zn_m_strdefs.h
zn_ck_fld.o : ../include/zn_maint.h ../include/zn_msg.h
zn_cl_fld.o : ../include/mad_error.h ../include/scr.h ../include/zn_ary.h
zn_cl_fld.o : ../include/zn_m_strdefs.h ../include/zn_maint.h
zn_maint.o : ../include/comm_strdefs.h ../include/key.h ../include/language.h
zn_maint.o : ../include/mad_error.h ../include/mads_types.h
zn_maint.o : ../include/zn_m_strdefs.h ../include/zn_maint.h
zn_maint.o : ../include/zn_msg.h
zn_sub1.o : ../include/df_maint.h ../include/ex_key.h ../include/language.h
zn_sub1.o : ../include/mad_error.h ../include/mads_isam.h
zn_sub1.o : ../include/mads_types.h ../include/tokenize.h
zn_sub1.o : ../include/zn_m_strdefs.h ../include/zn_maint.h ../include/zn_msg.h
zn_sub2.o : ../include/df_maint.h ../include/ex_key.h ../include/language.h
zn_sub2.o : ../include/mad_error.h ../include/mad_ipc.h ../include/mads_isam.h
zn_sub2.o : ../include/mads_types.h ../include/tokenize.h ../include/ui_ipc.h
zn_sub2.o : ../include/zn_m_strdefs.h ../include/zn_maint.h ../include/zn_msg.h
