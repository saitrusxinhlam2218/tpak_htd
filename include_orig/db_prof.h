
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  db_prof.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:44:52
* @(#)  Last delta: 12/2/94 at 18:25:42
* @(#)  SCCS File: /taxi/sccs/s.db_prof.h
*                                                                           *
* Copyright (c) 1991 - Motorola Seattle                                     *
*                                                                           *
*****************************************************************************/
#ident "@(#) head:db_prof.h	10.1.2.1"


/*
 * The value for N_DB_FILES comes from key.h
 * The value for N_DB_ACTIONS comes from mads_isam.h
 */
#ifdef DB_PROFILE


#define N_DB_FILES	39
#define N_DB_ACTIONS	18

#define DBD_SHM_KEY	777
#define DBU_SHM_KEY	888
#define MDT_SHM_KEY	101010

/* indexes into MDT shared memory storage array 	*/
/* This is where we keep the kdt stats information	*/
/* when we want to do dynamic system monitoring		*/
/* without making dispatch do all the work.			*/
/*													*/
#define MDT_SHM_SLOTS		100
#define RESPONSE_TIME		50
#define TOT_RESPONSE_TIME	51
#define MAX_RESPONSE_TIME	52
#define	NUM_RESPONSES		53
#define	NUM_TRANSACTIONS	54

#define MAKE_DB_MEM()			make_db_mem()
#define MDT_SHM_INC(a)          (*mdt_shmptr)[a]++
#define MDT_SHM_RES_TIME(a)     (*mdt_shmptr)[RESPONSE_TIME]     = (long)a
#define MDT_SHM_TOT_RES_TIME(a) (*mdt_shmptr)[TOT_RESPONSE_TIME] = (long)a
#define MDT_SHM_MAX_RES_TIME(a) (*mdt_shmptr)[MAX_RESPONSE_TIME] = (long)a
#define MDT_SHM_TRANSACTIONS(a) (*mdt_shmptr)[NUM_TRANSACTIONS]  = (long)a
#define DB_SHM_INC(a,b)			if (db_shmptr > 0 && a < N_DB_FILES && b < N_DB_ACTIONS) ++(*db_shmptr)[a][b]


#ifdef DECLARING
unsigned int (*db_shmptr)[N_DB_FILES][N_DB_ACTIONS];
long		 (*mdt_shmptr)[MDT_SHM_SLOTS];
int db_shmid   = 0;
int mdt_shmid  = 0;
#else
extern int db_shmid;
extern unsigned int (*db_shmptr)[][N_DB_ACTIONS];
extern int mdt_shmid;
extern long		 (*mdt_shmptr)[50];
#endif

#else
#define MAKE_DB_MEM()
#define MDT_SHM_INC(a)
#define MDT_SHM_RES_TIME(a)
#define MDT_SHM_TOT_RES_TIME(a)
#define MDT_SHM_MAX_RES_TIME(a)
#define MDT_SHM_TRANSACTIONS(a)
#define DB_SHM_INC(a,b)
#endif
