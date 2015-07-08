/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  mads_types.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:47:41
* @(#)  Last delta: 12/2/94 at 18:31:38
* @(#)  SCCS File: /taxi/sccs/s.mads_types.h
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/
/* Based on R3 version 5.05 */
#ifndef _MADS_TYPES_H
#define _MADS_TYPES_H
#ifndef _TAXIPAK_H_
#include "taxipak.h"
#endif
#ifndef _OBJECT_H_
#include "Object.h"
#endif
struct call_types {
  unsigned int passenger:1;
  unsigned int xtype:1;
  unsigned int delivery:1;
  unsigned int charge:1;
  unsigned int subscription:1;
  unsigned int wakeup:1;
  unsigned int time:1;
  unsigned int multiple:1;
  unsigned int all_3_call_types:1; 
  unsigned int ilink:1;
  unsigned int ilink_reject:1;
};
struct seals_flags {			/* seals for taxis and drivers */
	unsigned s1 : 1;
	unsigned s2 : 1;
	unsigned s3 : 1;
	unsigned s4 : 1;
	unsigned s5 : 1;
	unsigned s6 : 1;
	unsigned s7 : 1;
	unsigned s8 : 1;
	unsigned s9 : 1;
	unsigned s10 : 1;
	unsigned s11 : 1;
	unsigned s12 : 1;
	unsigned s13 : 1;
	unsigned s14 : 1;
	unsigned s15 : 1;
	unsigned s16 : 1;
} ;
struct veh_attributes {		/* attributes for vehicles */
	unsigned attr1 : 1;
	unsigned attr2 : 1;
	unsigned attr3 : 1;
	unsigned attr4 : 1;
	unsigned attr5 : 1;
	unsigned attr6 : 1;
	unsigned attr7 : 1;
	unsigned attr8 : 1;
	unsigned attr9 : 1;
	unsigned attr10 : 1;
	unsigned attr11: 1;
	unsigned attr12: 1;
	unsigned attr13: 1;
	unsigned attr14: 1;
	unsigned attr15: 1;
	unsigned attr16: 1;
	unsigned attr17: 1;
	unsigned attr18: 1;
	unsigned attr19: 1;
	unsigned attr20: 1;
	unsigned attr21: 1;
	unsigned attr22: 1;
	unsigned attr23: 1;
	unsigned attr24: 1;
	unsigned attr25: 1;
	unsigned attr26: 1;
	unsigned attr27: 1;
	unsigned attr28: 1;
	unsigned attr29: 1;
	unsigned attr30: 1;
	unsigned attr31: 1;
	unsigned attr32: 1;	
	unsigned susp_pass_call : 1;	/* allowed to pickup passenger calls */
	unsigned susp_del_call : 1;  	/* allowed to pickup delivery calls */
	unsigned susp_typex_call : 1;  	/* allowed to pickup  x calls */
};
struct drv_attributes {		/* attributes for drivers */
	unsigned attr1 : 1;
	unsigned attr2 : 1;
	unsigned attr3 : 1;
	unsigned attr4 : 1;
	unsigned attr5 : 1;
	unsigned attr6 : 1;
	unsigned attr7 : 1;
	unsigned attr8 : 1;
	unsigned attr9 : 1;
	unsigned attr10 : 1;
	unsigned attr11: 1;
	unsigned attr12: 1;
	unsigned attr13: 1;
	unsigned attr14: 1;
	unsigned attr15: 1;
	unsigned attr16: 1;
	unsigned attr17: 1;
	unsigned attr18: 1;
	unsigned attr19: 1;
	unsigned attr20: 1;
	unsigned attr21: 1;
	unsigned attr22: 1;
	unsigned attr23: 1;
	unsigned attr24: 1;
	unsigned attr25: 1;
	unsigned attr26: 1;
	unsigned attr27: 1;
	unsigned attr28: 1;
	unsigned attr29: 1;
	unsigned attr30: 1;
	unsigned attr31: 1;
	unsigned attr32: 1;		
	unsigned susp_pass_call : 1;	/* allowed to pickup passenger calls */
	unsigned susp_del_call : 1;  	/* allowed to pickup delivery calls */
	unsigned susp_typex_call : 1;  	/* allowed to pickup  x calls */
	unsigned need_attr : 1;		/* whether a call needs special attributes */
};
struct taxi_status {
	unsigned posted : 1;		/* driver is posted */
	unsigned e_posted : 1;		/* driver is enroute posted */
	unsigned on_break : 1;		/* driver is on break */
	unsigned accepted : 1;		/* driver has accepted a call */
	unsigned pckup : 1;		/* driver has picked up a fare */
	unsigned idle : 1;		/* driver is unposted */
	unsigned offered : 1;		/* driver is being offered a call */
	unsigned inuse : 1;		/* taxi is in use */
	unsigned kdt_working : 1; 	/* KDT-440 terminal functional  */
	unsigned meter_working : 1; 	/* vehicle's meter is functional */
	unsigned veh_susp : 1;		/* vehicle is suspended from passenger calls*/
	unsigned drv_susp : 1;		/* driver is suspended from passenger calls*/
	unsigned flag:1;		/* driver is on a flag call */
	unsigned noshow:1;		/* vehicle is in noshow waiting for approval */
	unsigned emer:1;		/* vehicle is in emergency state */
	unsigned emer_clear:1;          /* vehicle had an emergency and it was cleared by dispatcher */
	unsigned late_meter:1;		/* vehicle has a late meter */
	unsigned local:1;		/* vehicle on a local call */
	unsigned meter_on:1;
	unsigned veh_susp_d:1;		/* vehicle is suspended from delivery calls*/
	unsigned veh_susp_x:1;		/* vehicle is suspended from type x calls*/
	unsigned drv_susp_d:1;		/* driver is suspended from delivery calls*/
	unsigned drv_susp_x:1;		/* driver is suspended from type x calls*/
	unsigned crse_annul:1;
	unsigned :1;
	unsigned :1;
};
/*
	this bit stucture must be in the same order as the defines for the exception types
*/
struct	except_bits {
		unsigned emergency:1;	/* emergency		*/
		unsigned call_cancel:1;	/* call cancel		*/
		unsigned unzoned:1;	/* unzoned call		*/
		unsigned call_msg:1;	/* call with message 	*/
		unsigned call_no_msg:1;	/* call w/o message 	*/
		unsigned extra_taxis:1;	/* extra taxis		*/
		unsigned callback:1;	/* callback		*/
		unsigned call_tmout:1;	/* call timeout		*/
		unsigned reject:1;	/* call in process of being rejected */
		unsigned late_meter:1;	/* late meter on 	*/
		unsigned flag_req:1;	/* flag request		*/
		unsigned no_show:1;	/* No Show		*/
		unsigned callout:1;	/* Callout		*/
		unsigned req_talk:1;	/* request to talk	*/
		unsigned eta:1;		/* ETA			*/
		unsigned msgs:1;	/* messages		*/
		unsigned bid:1;		/* call put for bid	*/
		unsigned dr_msg:1;	/* driver message	*/
		unsigned wakeup:1;	/* W - Wake-up call.  */
		unsigned short_meter:1;	/* short meter on off	*/
		unsigned holiday:1;    	/* holiday permanent time call*/
		unsigned sys_error:1;	/* system errors	*/
		unsigned sup_hold:1;	/* Supervisor HOLD */
		unsigned manual:1;	/* MANUAL */
		unsigned personal:1;	/* PERSONAL */
		unsigned mfault:1;	/* MFAULT */
		unsigned multiple:1;
		unsigned rcpt_missing:1; /* exhausted retrys for missing receipts */
		unsigned itm_invoice_nak:1;
		unsigned attr_hold:1;
		unsigned ns_short:1;
		unsigned :1;
/* start of status */
		unsigned offered:1;	/* call is being offered */
		unsigned available:1;	/* call is available for dispatch */
		unsigned update:1;	/* update call		*/
		unsigned local:1;	/* local call 		*/
		unsigned upforbid:1;	/* call is up for bid */
		unsigned holdline:1;	/* passenger is on the line */
		unsigned wait_epost:1;	/* waiting for epost to post */
		unsigned wait_primary_epost:1; /*waiting in primary zone for epost to post*/
		unsigned accepted:1;
		unsigned pickup:1;
		unsigned pers_attr_override:1;
};
/* The exception count structure is a 3D array with axes for each fleet,
each type of exception, and each super zone index plus an additional space
for the exceptions without super zones (e.g. WAKEUP or UNZONED) and
the exceptions without fleets */
struct exception_count_struct {
  short count[ /*FLEET_MAX + 1*/ 9][ /*EXCEPT_TYPE_MAX*/ 32][ /* MAX_EXC_TYPE + 1(# of superzones + 1)*/ 17];
  long time;
};
typedef struct _MANUAL_FLAT_HIST_STRUCT
{
  short        ManualFlatPerformed;
  float        PrevFlatRate;
  short        UserID;
} MANUAL_FLAT_HIST;
#endif
