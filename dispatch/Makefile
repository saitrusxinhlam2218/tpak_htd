#/****************************************************************************
#*                                                                           *
#*                           S C C S   I N F O                               *
#*                                                                           *
#* @(#)  dispatch.mk; Rel: 10.1.2.2; Get date: 5/24/95 at 14:49:13		            *			
#* @(#)  Last delta: 5/24/95 at 14:49:03						    *	
#* @(#)  SCCS File: /taxi/sccs/s.dispatch.mk							    *
#*                                                                           *
#* Copyright (c) 1994 - TaxiCom ApS					    *
#*                                                                           *
#*****************************************************************************/
#ident "@(#) head:dispatch.mk	10.1.2.2"

#/* Based on R3 version 5.17 */
include ../Makefile.inc
ENH=../enhance_lib


LIBFLAGS = -L../lib -L$(ENH) -lm -ldb -lfill -lcurses -lCall -lcommon -lFleet -lList -ldbenhance $(CISAMDB_LIBS)

OBJ =   init.o ui_dispatch.o \
	dispatch.o kdt_1.o kdt_2.o kdt_3.o kdt_4.o kdt_5.o kdt_init.o writer.o msg_dfuncs.o \
	do_warm.o drv_stat.o out_msg.o msg_mmp.o inactivity_logout.o word_break.o \
	acct_verify.o itm.o itm_receipt.o  \
	chk_midnite.o ccp_misc.o sp_misc.o squeeze.o trouble.o \
	Timer.o \
	Call.o Call_timers.o Call_private.o \
	except.o exception_distribute.o exception_action.o \
	Fleet.o Fleet_private.o \
	Vehicle.o Vehicle_lists.o Vehicle_timers.o Vehicle_private.o Vehicle_key.o \
	Driver.o Driver_private.o \
	Zone.o Zone_ext.o Zone_private.o \
	TimeCall_private.o \
	Dispatcher.o GPS_private.o Call_msg_private.o pkt_id.o Tracking_Layer_private.o InterLink_private.o \
	ZoneDown.o strmap.o

dispatch: $(OBJ) 
	xlc $(CFLAGS) $(OBJ) $(LIBFLAGS) -o dispatch



