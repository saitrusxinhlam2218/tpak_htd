#/****************************************************************************
#*                                                                           *
#*                           S C C S   I N F O                               *
#*                                                                           *
#* @(#)  ace.mk; Rel: 10.1.1.1; Get date: 6/2/95 at 12:46:20
#* @(#)  Last delta: 12/2/94 at 15:34:59
#* @(#)  SCCS File: /taxi/sccs/s.ace.mk
#*                                                                           *
#* Copyright (c) 1990 - MDI Seattle                                          *
#*                                                                           *
#*****************************************************************************/
#ident "@(#) head:ace.mk	10.1.1.1"

AC=saceprep

all: finnish

all: da_cabs.arc da_charg.arc da_autob.arc da_drive.arc da_drmes.arc da_emerg.arc da_su_dr.arc da_su_ve.arc da_symes.arc da_unzon.arc da_usest.arc da_usum.arc da_vehic.arc de_drive.arc de_autob.arc de_live.arc de_subsc.arc de_unasg.arc de_vehic.arc messages.arc

da_cabs.arc: da_cabs.ace
	cd finnish ; $(AC) da_cabs.ace ; \
	cd ../danish ; $(AC) da_cabs.ace ; \
	cd ../swedish ; $(AC) da_cabs.ace ; \
	cd ../english ; $(AC) da_cabs.ace 

da_charg.arc: da_charg.ace
	cd finnish ; $(AC) da_charg.ace ; \
	cd ../danish ; $(AC) da_charg.ace ; \
	cd ../swedish ; $(AC) da_charg.ace ; \
	cd ../english ; $(AC) da_charg.ace

da_autob.arc: da_autob.ace
	cd finnish ; $(AC) da_autob.ace ; \
	cd ../danish ; $(AC) da_autob.ace ; \
	cd ../swedish ; $(AC) da_autob.ace ; \
	cd ../english ; $(AC) da_autob.ace 

da_drive.arc: da_drive.ace
	cd finnish ; $(AC) da_drive.ace ; \
	cd ../danish ; $(AC) da_drive.ace ; \
	cd ../swedish ; $(AC) da_drive.ace ; \
	cd ../english ; $(AC) da_drive.ace 

da_drmes.arc: da_drmes.ace
	cd finnish ; $(AC) da_drmes.ace ; \
	cd ../danish ; $(AC) da_drmes.ace ; \
	cd ../swedish ; $(AC) da_drmes.ace ; \
	cd ../english ; $(AC) da_drmes.ace 

da_emerg.arc: da_emerg.ace
	cd finnish ; $(AC) da_emerg.ace ; \
	cd ../danish ; $(AC) da_emerg.ace ; \
	cd ../swedish ; $(AC) da_emerg.ace ; \
	cd ../english ; $(AC) da_emerg.ace 

da_su_dr.arc: da_su_dr.ace
	cd finnish ; $(AC) da_su_dr.ace ; \
	cd ../danish ; $(AC) da_su_dr.ace ; \
	cd ../swedish ; $(AC) da_su_dr.ace ; \
	cd ../english ; $(AC) da_su_dr.ace

da_su_ve.arc: da_su_ve.ace
	cd finnish ; $(AC) da_su_ve.ace ; \
	cd ../danish ; $(AC) da_su_ve.ace ; \
	cd ../swedish ; $(AC) da_su_ve.ace ; \
	cd ../english ; $(AC) da_su_ve.ace 

da_symes.arc: da_symes.ace
	cd finnish ; $(AC) da_symes.ace ; \
	cd ../danish ; $(AC) da_symes.ace ; \
	cd ../swedish ; $(AC) da_symes.ace ; \
	cd ../english ; $(AC) da_symes.ace 

da_unzon.arc: da_unzon.ace
	cd finnish ; $(AC) da_unzon.ace ; \
	cd ../danish ; $(AC) da_unzon.ace ; \
	cd ../swedish ; $(AC) da_unzon.ace ; \
	cd ../english ; $(AC) da_unzon.ace 

da_usest.arc: da_usest.ace
	cd finnish ; $(AC) da_usest.ace ; \
	cd ../danish ; $(AC) da_usest.ace ; \
	cd ../swedish ; $(AC) da_usest.ace ; \
	cd ../english ; $(AC) da_usest.ace 

da_usum.arc: da_usum.ace
	cd finnish ; $(AC) da_usum.ace ; \
	cd ../danish ; $(AC) da_usum.ace ; \
	cd ../swedish ; $(AC) da_usum.ace ; \
	cd ../english ; $(AC) da_usum.ace 

da_vehic.arc: da_vehic.ace
	cd finnish ; $(AC) da_vehic.ace ; \
	cd ../danish ; $(AC) da_vehic.ace ; \
	cd ../swedish ; $(AC) da_vehic.ace ; \
	cd ../english ; $(AC) da_vehic.ace 

de_drive.arc: de_drive.ace
	cd finnish ; $(AC) de_drive.ace ; \
	cd ../danish ; $(AC) de_drive.ace ; \
	cd ../swedish ; $(AC) de_drive.ace ; \
	cd ../english ; $(AC) de_drive.ace 

de_autob.arc: de_autob.ace
	cd finnish ; $(AC) de_autob.ace ; \
	cd ../danish ; $(AC) de_autob.ace ; \
	cd ../swedish ; $(AC) de_autob.ace ; \
	cd ../english ; $(AC) de_autob.ace 

de_live.arc: de_live.ace
	cd finnish ; $(AC) de_live.ace ; \
	cd ../danish ; $(AC) de_live.ace ; \
	cd ../swedish ; $(AC) de_live.ace ; \
	cd ../english ; $(AC) de_live.ace 

de_subsc.arc: de_subsc.ace
	cd finnish ; $(AC) de_subsc.ace ; \
	cd ../danish ; $(AC) de_subsc.ace ; \
	cd ../swedish ; $(AC) de_subsc.ace ; \
	cd ../english ; $(AC) de_subsc.ace 

de_unasg.arc: de_unasg.ace
	cd finnish ; $(AC) de_unasg.ace ; \
	cd ../danish ; $(AC) de_unasg.ace ; \
	cd ../swedish ; $(AC) de_unasg.ace ; \
	cd ../english ; $(AC) de_unasg.ace 

de_vehic.arc: de_vehic.ace
	cd finnish ; $(AC) de_vehic.ace ; \
	cd ../danish ; $(AC) de_vehic.ace ; \
	cd ../swedish ; $(AC) de_vehic.ace ; \
	cd ../english ; $(AC) de_vehic.ace 

messages.arc: messages.ace
	cd finnish ; $(AC) messages.ace ; \
	cd ../danish ; $(AC) messages.ace ; \
	cd ../swedish ; $(AC) messages.ace ; \
	cd ../english ; $(AC) messages.ace 


