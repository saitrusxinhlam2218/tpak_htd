/*****************************************************************************
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  switch_enh.h; Rel: 10.1.0.0; Get date: 4/18/95 at 16:36:26		            *			
* @(#)  Last delta: 12/2/94 at 12:15:35						    *	
* @(#)  SCCS File: /taxi/sccs/s.switch_enh.h							    *
*                                                                           *
* Copyright (c) 1994 - TaxiCom ApS					    *
*                                                                           *
*****************************************************************************/

#define	CFG_FILE	"/usr/taxi/run/enh.cfg"

/* Finland credit card system */
#define ENH0001   0x01

/* SamPlan system */
#define ENH0002   0x02

/* Trip Details resend on Approval of ASSIGN_ERR exception */
#define ENH0003   0x04

/* Call Comments */
#define ENH0004   0x08

/* Customer Profiles  */
#define ENH0005   0x10

/* Reservation Periods */
#define ENH0006   0x20

/* 32/32 Attributes */
#define ENH0007   0x40

/* Stockholm Pkg 97-01 */
#define ENH0008   0x80

/* Advanced Credit Card Processing */
#define ENH0009   0x100

/* R4.2 Enhancements               */
#define ENH0010   0x200

/* Extended Call Messages          */
#define ENH0011   0x400

/* Koge bucket                     */
#define ENH0012   0x800

/* Special SIGN-ON Procedure - Testing */
#define ENH0013   0x1000

/* Screen 32 Attribute Filters     */
#define ENH0014   0x2000

/* Vehicle to Vehicle Messages     */
#define ENH0015   0x4000

/* Black & White Taxis Bucket      */
#define ENH0016   0x8000

/* Dual ACC                        */
#define ENH0017   0x10000          

/* Message Filters by Attribute    */
#define ENH0018   0x20000

/* Call Entry Features HOLD and SWITCH */
#define ENH0019   0x40000

/* TimeCall First Priority Dispatch */
#define ENH0020   0x80000

/* Subscription Call Changes        */
#define ENH0021   0x100000

/* Vehicle Details on Screen 33     */
#define ENH0022   0x200000

/* R4.4 Enhancements                */
#define ENH0023   0x400000

/* TaxiGEO */
#define ENH0024   0x800000
