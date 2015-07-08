 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  mad_ipc.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:47:31
 * @(#)  Last delta: 12/2/94 at 18:17:46
 * @(#)  SCCS File: /taxi/sccs/s.mad_ipc.h
 *                                                                           *
 *****************************************************************************/
#ifndef _MAD_IPC_H
#define _MAD_IPC_H
#define	SEMKEY	2				/* semaphore key */
#define SHMKEY	3				/* shared memory key */
#define SHMKEY2  4                               /* time calls shared memory key */
#define SHMKEY_MAP_ADDR 0x0a00000010000000
#define MSGKEY	2048				/* message key id */
#define PI_MSGKEY 2049                          /* message queue for PI messaging */
#define SHARED_START	NULL			/* beginning of shared memory */
#define	NSEMS	2				/* # of semaphores in the semaphore set */
struct tpk_stat {
	unsigned int dispatch:1;	/* dispatch is running */
	unsigned int ncp:1;		/* ncp is running */
	unsigned int primary:1;		/* primary host */
	unsigned int redncy:1;		/* redundancy state */
	unsigned int bid_list_full:1;	/* bid list full */
  unsigned int load_unload:1;     /* MDT load-unload enabled */
	unsigned int s7:1;
	unsigned int s8:1;	
	unsigned int s9:1;	
	unsigned int s10:1;	
	unsigned int s11:1;	
	unsigned int s12:1;	
	unsigned int poll_dispatch:1;	/* dispatch status for monitor */
	unsigned int poll_pi:1;	        /* pi status for monitor */
	unsigned int poll_autobook:1;	/* autobook status for monitor */
	unsigned int poll_ccp:1;	/* credit card processor (TAMPERE) status for monitor */
};
typedef struct _HOLIDAY_
{
  int          mon;
  int          day;
  int          year;
} HOLIDAYS;
#define MAX_HOLIDAYS 100
#define HOLIDAY_MEM_SIZE ( MAX_HOLIDAYS * sizeof( HOLIDAYS ) )
#define VEH_DRIV_SIZE  ( VEH_DRIV_MAX * sizeof( struct veh_driv ) )
#define VEH_TABLE_SIZE ( VEH_DRIV_MAX * sizeof( struct veh_index ) )
typedef struct _TLG_STRUCT
{
  int         lo_range;
  int         hi_range;
  char        fleet;
} TLG_ENTRY;
#define MAX_TLG    50
#define TLG_MEM_SIZE  ( MAX_TLG * sizeof( TLG_ENTRY ) )
typedef struct _CHANNEL_STRUCT
{
  char        channel_id[3];
  int         channel_nbr;
  char        channel_short_descript[4];
  char        channel_descript[32];
} CHANNEL_ENTRY;
#define MAX_CHANNELS      18
#define CHANNEL_MEM_SIZE ( MAX_CHANNELS * sizeof( CHANNEL_ENTRY ) )
struct offsets { 			/* offsets of structures in shared memory */
	struct fleets *fleet;		/* pointer to fleet structure */
	struct calls *call;		/* pointer to call structure */
	struct users *user;		/* pointer to user  structure */
	struct excepts *except;		/* pointer to except structure */
	struct excpt_desc *except_desc;	/* pointer to except description structure */
	struct menus *menu;		/* pointer to menu structure */
	struct excepts *first_except;	/* pointer to first exception in the exception table */
	struct excepts *free_except; 	/* pointer to next available exception */
	struct list_struct   *free_excpt_lists;
	struct list_elements *free_excpt_elements;
	struct calls *free_call;	/* pointer to next availble slot in call table */
	int free_call_idx;		/* next availble slot in call table index */
	int free_except_idx;		/* next availble slot in call table index */
	int veh_used;                   /* number of vehicles registered in system */
	struct tpk_stat status;		/* system status word */
	int prattle;			/* level of prattle in trace statements */
	int stat_update;		/* time statistics should be updated */
	unsigned long mm_ul_cmd;	/* cmd = upper 16-bits; pid = lower 16-bits */
	struct exception_count_struct *except_count;
	struct res_periods        *res_period;
	HOLIDAYS    *Holidays;
#ifdef NAMERICA
	TLG_ENTRY   *tlg_groups;
	CHANNEL_ENTRY *channels;
#endif
};
#endif
