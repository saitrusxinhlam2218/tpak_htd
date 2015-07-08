 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  acc.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:44:11
 * @(#)  Last delta: 12/2/94 at 18:21:07
 * @(#)  SCCS File: /taxi/sccs/s.acc.h
 *                                                                           *
 * Copyright (c) 1990 - MDI Seattle                                          *
 *                                                                           *
 *****************************************************************************/
/** Test.. **/
#define	ACC_KEY		0x12345678	/* shmget key stats */
#define TO_ACC_KEY	0x52656376	/* IPC msgget key for sending to ACC ("Send") */
#define FROM_ACC_KEY	0x52656386	/* IPC msgget key for receiveing from ACC ("Recv") */
#define MID_SIZE	5		/* id field length */
#define	P_FLD_SIZE	5		/* Parmeter field size */
#define ALC_STAT_KEY    22801 
