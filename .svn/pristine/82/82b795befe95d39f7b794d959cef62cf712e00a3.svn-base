#***********************************************************************
#                 RCS INFO
#
#  @(#)  $RCSfile: Makefile,v $
#  @(#)  $Revision: 1.16 $
#  @(#)  $Date: 2004/08/23 10:23:58 $
#  @(#)  $Author: jwelch $
#  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill/Makefile,v $
#
#  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
#
#*********************************************************************/
#ident "@(#) head:$RCSfile: Makefile,v $	$Revision: 1.16 $"

include ../Makefile.inc
SRC =..
IN1 = /usr/include

OBJS =   fill_ac.o   fill_ad.o   fill_at.o \
				 fill_ch.o   fill_cl.o   fill_dr.o \
				 fill_eg.o   fill_el.o   fill_em.o \
				 fill_et.o   fill_fl.o   fill_hl.o \
				 fill_hp.o   fill_in.o   fill_lo.o \
				 fill_mb.o   fill_mf.o   fill_ml.o \
				 fill_pl.o   fill_rm.o   fill_rt.o \
				 fill_sc.o   fill_sl.o   fill_sn.o \
				 fill_st.o   fill_syc.o  fill_sz.o \
				 fill_up.o   fill_uz.o   fill_vh.o fill_imsi.o \
				 fill_zone.o fill_su.o   fill_sd.o \
				 fill_cm.o   fill_sa.o   fill_hh.o \
				 fill_cmsg.o \
				 fill_cust.o fill_hist_cmt.o fill_res.o \
				 fill_time_call.o fill_tlg.o fill_hia.o fill_acad.o fill_indv.o \
				 fill_rd.o fill_bad_credit.o fill_card_type.o fill_gps_params.o \
				 fill_sysmenuitems.o fill_sysmenus.o fill_vh_cmsg.o fill_tl.o fill_subch.o fill_zonepoly.o \
				 fill_webuser.o fill_webaddr.o fill_ftj_bad.o fill_lm.o fill_geoaddr_addr.o \
				fill_geoaddr_line.o fill_geoaddr_point.o fill_attrans.o fill_zntrans.o fill_ilink.o fill_ss.o fill_sh.o fill_zonedown.o fill_kelanode.o


libfill.a : $(OBJS) 
	ar ru libfill.a $(OBJS) ;\
	ranlib libfill.a ;\
	cp libfill.a $(SRC)/lib 

lint :
	lint -I../include -o fill *.c

fill_ac.o : ../include/mads_isam.h ../include/mads_types.h
fill_ad.o : ../include/df_maint.h ../include/mads_isam.h
fill_ad.o : ../include/mads_types.h
fill_at.o : ../include/mads_isam.h ../include/mads_types.h
fill_ch.o : ../include/df_maint.h ../include/mads_isam.h
fill_ch.o : ../include/mads_types.h
fill_cl.o : ../include/df_maint.h ../include/key_offset.h
fill_cl.o : ../include/mads_isam.h ../include/mads_types.h
fill_cm.o : ../include/mads_isam.h ../include/mads_types.h
fill_cmsg.o : ../include/mads_isam.h ../include/mads_types.h
fill_dr.o : ../include/key_offset.h ../include/mads_isam.h
fill_dr.o : ../include/mads_types.h
fill_eg.o : ../include/mads_isam.h ../include/mads_types.h
fill_el.o : ../include/df_maint.h ../include/mads_isam.h
fill_el.o : ../include/mads_types.h
fill_em.o : ../include/df_maint.h ../include/mads_isam.h
fill_em.o : ../include/mads_types.h
fill_et.o : ../include/mads_isam.h ../include/mads_types.h
fill_fl.o : ../include/key_offset.h ../include/mads_isam.h
fill_fl.o : ../include/mads_types.h
fill_hh.o : ../include/df_maint.h ../include/mads_isam.h
fill_hh.o : ../include/mads_types.h
fill_hl.o : ../include/mads_isam.h ../include/mads_types.h
fill_hp.o : ../include/mads_isam.h ../include/mads_types.h
fill_in.o : ../include/mads_isam.h ../include/mads_types.h
fill_lo.o : ../include/mads_isam.h ../include/mads_types.h
fill_mb.o : ../include/df_maint.h ../include/mads_isam.h
fill_mb.o : ../include/mads_types.h
fill_mf.o : ../include/df_maint.h ../include/mads_isam.h
fill_mf.o : ../include/mads_types.h
fill_ml.o : ../include/df_maint.h ../include/mads_isam.h
fill_ml.o : ../include/mads_types.h
fill_pl.o : ../include/df_maint.h ../include/mads_isam.h
fill_pl.o : ../include/mads_types.h
fill_rm.o : ../include/df_maint.h ../include/mads_isam.h
fill_rm.o : ../include/mads_types.h
fill_rt.o : ../include/mads_isam.h ../include/mads_types.h
fill_sa.o : ../include/mads_isam.h ../include/mads_types.h
fill_sc.o : ../include/df_maint.h ../include/key_offset.h
fill_sc.o : ../include/mads_isam.h ../include/mads_types.h
fill_sd.o : ../include/df_maint.h ../include/mads_isam.h
fill_sd.o : ../include/mads_types.h
fill_sh.o : ../include/df_maint.h ../include/key_offset.h
fill_sh.o : ../include/mads_isam.h ../include/mads_types.h
fill_sl.o : ../include/mads_isam.h ../include/mads_types.h
fill_sn.o : ../include/df_maint.h ../include/mads_isam.h
fill_sn.o : ../include/mads_types.h
fill_ss.o : ../include/key_offset.h ../include/mads_isam.h
fill_ss.o : ../include/mads_types.h
fill_st.o : ../include/mads_isam.h ../include/mads_types.h
fill_su.o : ../include/df_maint.h ../include/mads_isam.h
fill_su.o : ../include/mads_types.h
fill_syc.o : ../include/mads_isam.h ../include/mads_types.h
fill_sz.o : ../include/mads_isam.h ../include/mads_types.h
fill_up.o : ../include/mads_isam.h ../include/mads_types.h
fill_uz.o : ../include/mads_isam.h ../include/mads_types.h
fill_vh.o : ../include/key_offset.h ../include/mads_isam.h
fill_vh.o : ../include/mads_types.h
fill_zone.o : ../include/mads_isam.h ../include/mads_types.h

