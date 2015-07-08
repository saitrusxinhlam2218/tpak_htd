#/***********************************************************************
#*                 RCS INFO
#*
#*  @(#)  $RCSfile: Makefile,v $
#*  @(#)  $Revision: 1.2 $
#*  @(#)  $Date: 1998/06/18 15:28:47 $
#*  @(#)  $Author: taxiadm $
#*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/msf/fin/Makefile,v $
#*
#*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
#*
#***********************************************************************/
#ident "@(#) head:$RCSfile: Makefile,v $	$Revision: 1.2 $"
#
#static char rcsid[] = "$Id: Makefile,v 1.2 1998/06/18 15:28:47 taxiadm Exp $";

.SUFFIXES: .MSF .msf .MSF8

MSF8_OBJECTS = chosfleets.MSF8 adminmgmt.MSF8 common.MSF8 dr_maint.MSF8 filemgmt.MSF8 fl_maint.MSF8 geo_maint.MSF8 in_maint.MSF8 literals.MSF8 \
	      menu_maint.MSF8 sysmain.MSF8 sysmgmt.MSF8 ui.MSF8 up_maint.MSF8 vh_maint.MSF8 zn_maint.MSF8 au_live.MSF8 au_message.MSF8 \
	      au_trail.MSF8 choosedttm.MSF8 chosfleets.MSF8 da_cabs_hr.MSF8 da_calbak.MSF8 da_calls.MSF8 da_charge.MSF8 da_manual.MSF8 \
	      da_report.MSF8 da_shift.MSF8 \
              da_usesum.MSF8 da_view.MSF8 da_zones.MSF8 de_actveh.MSF8 de_drive.MSF8 de_live.MSF8 de_lmain.MSF8 de_manifst.MSF8 \
              de_mvehic.MSF8 de_subsc.MSF8 de_umain.MSF8 de_unasig.MSF8 de_vehic.MSF8 getsyscntl.MSF8 intervals.MSF8 man_veh.MSF8 \
              report_h.MSF8 msg_text.MSF8 f_msg_defs.MSF8 writer.MSF8 shift_s.MSF8 print_end.MSF8 credit_main.MSF8 da_credit.MSF8 \
              dispatch.MSF8 syncdb.MSF8 hol_maint.MSF8 res_per.MSF8  ccp.MSF8
              

MSF_OBJECTS = adminmgmt.msf common.msf dr_maint.msf filemgmt.msf fl_maint.msf geo_maint.msf in_maint.msf literals.msf \
	      menu_maint.msf sysmain.msf sysmgmt.msf ui.msf up_maint.msf vh_maint.msf zn_maint.msf au_live.msf au_message.msf \
	      au_trail.msf choosedttm.msf chosfleets.msf da_cabs_hr.msf da_calbak.msf da_calls.msf da_charge.msf da_manual.msf \
	      da_report.msf da_shift.msf \
              da_usesum.msf da_view.msf da_zones.msf de_actveh.msf de_drive.msf de_live.msf de_lmain.msf de_manifst.msf \
              de_mvehic.msf de_subsc.msf de_umain.msf de_unasig.msf de_vehic.msf getsyscntl.msf intervals.msf man_veh.msf \
              report_h.msf msg_text.msf f_msg_defs.msf writer.msf shift_s.msf print_end.msf credit_main.msf da_credit.msf \
              dispatch.msf syncdb.msf hol_maint.msf  res_per.msf ccp.msf

all:	adminmgmt.cat common.cat dr_maint.cat file_mgmt.cat fl_maint.cat geo_maint.cat in_maint.cat literals.cat menu_maint.cat \
        system_main.cat system_mgmt.cat ui.cat up_maint.cat vh_maint.cat zn_maint.cat au_live.cat au_trail.cat da_purge.cat \
        da_report.cat da_view.cat de_actveh.cat de_drive.cat de_live.cat de_lmain.cat de_manifst.cat de_mvehic.cat de_subsc.cat \
        de_umain.cat de_unasig.cat de_vehic.cat dispatch.cat writer.cat shift_s.cat credit_main.cat ccp.cat

7to8_convert: $(MSF8_OBJECTS)
		

adminmgmt.cat: $(MSF_OBJECTS)
	gencat adminmgmt.cat adminmgmt.MSF.msf

common.cat:
	gencat common.cat common.MSF.msf

dr_maint.cat:
	gencat dr_maint.cat dr_maint.MSF.msf

file_mgmt.cat:
	gencat file_mgmt.cat filemgmt.MSF.msf

fl_maint.cat:
	gencat fl_maint.cat fl_maint.MSF.msf

geo_maint.cat:
	gencat geo_maint.cat geo_maint.MSF.msf

in_maint.cat:
	gencat in_maint.cat in_maint.MSF.msf

literals.cat:
	gencat literals.cat literals.MSF.msf

menu_maint.cat:
	gencat menu_maint.cat menu_maint.MSF.msf

system_main.cat:
	gencat system_main.cat sysmain.MSF.msf

system_mgmt.cat:
	gencat system_mgmt.cat sysmgmt.MSF.msf

ui.cat:	
	gencat ui.cat ui.MSF.msf

up_maint.cat:
	gencat up_maint.cat up_maint.MSF.msf

vh_maint.cat:
	gencat vh_maint.cat vh_maint.MSF.msf

zn_maint.cat:
	gencat zn_maint.cat zn_maint.MSF.msf

au_live.cat:
	gencat au_live.cat au_live.MSF.msf

au_trail.cat:
	gencat au_trail.cat au_trail.MSF.msf

da_purge.cat:
	gencat da_purge.cat chosfleets.MSF.msf report_h.MSF.msf print_end.MSF.msf

da_report.cat:
	gencat da_report.cat da_report.MSF.msf chosfleets.MSF.msf da_cabs_hr.MSF.msf da_calbak.MSF.msf da_calls.MSF.msf \
	da_manual.MSF.msf da_zones.MSF.msf da_shift.MSF.msf report_h.MSF.msf print_end.MSF.msf da_credit.MSF.msf

da_view.cat:
	gencat da_view.cat da_view.MSF.msf chosfleets.MSF.msf

de_actveh.cat:
	gencat de_actveh.cat de_actveh.MSF.msf

de_drive.cat:
	gencat de_drive.cat de_drive.MSF.msf chosfleets.MSF.msf

de_live.cat:
	gencat de_live.cat de_live.MSF.msf chosfleets.MSF.msf choosedttm.MSF.msf

de_lmain.cat:
	gencat de_lmain.cat de_lmain.MSF.msf chosfleets.MSF.msf choosedttm.MSF.msf

de_manifst.cat:
	gencat de_manifst.cat de_manifst.MSF.msf

de_mvehic.cat:
	gencat de_mvehic.cat de_mvehic.MSF.msf chosfleets.MSF.msf

de_subsc.cat:
	gencat de_subsc.cat de_subsc.MSF.msf chosfleets.MSF.msf

de_umain.cat:
	gencat de_umain.cat de_umain.MSF.msf chosfleets.MSF.msf choosedttm.MSF.msf

de_unasig.cat:
	gencat de_unasig.cat de_unasig.MSF.msf chosfleets.MSF.msf choosedttm.MSF.msf

de_vehic.cat:
	gencat de_vehic.cat de_vehic.MSF.msf chosfleets.MSF.msf choosedttm.MSF.msf

dispatch.cat:
	gencat dispatch.cat msg_text.MSF.msf f_msg_defs.MSF.msf dispatch.MSF.msf

writer.cat:
	gencat writer.cat writer.MSF.msf

shift_s.cat:
	gencat shift_s.cat shift_s.MSF.msf

credit_main.cat:
	gencat credit_main.cat credit_main.MSF.msf

ccp.cat:
	gencat ccp.cat ccp.MSF.msf



.MSF.msf:
	mkcatdefs -h dummy $< > $<.msf

.MSF.MSF8:
	sed "s/\|/ö/g" $< | sed "s/{/ä/g" | sed "s/}/å/g" | sed "s/]/Å/g" | sed "s/\[/Ä/g" | \
	sed "s/\^/Ü/g" | sed "s/\~/ü/g" | sed "s/\\\\\\\/Ö/g"> $<.MSF8
	

clean:
	rm *.msf *.cat


