 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  syncdb.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:49:42
 * @(#)  Last delta: 12/2/94 at 18:35:04
 * @(#)  SCCS File: /taxi/sccs/s.syncdb.h
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:syncdb.h	10.1.2.1"

#define DAT_SUFFIX      ".dat"       /* isam file: .dat suffix */
#define IDX_SUFFIX      ".idx"       /* isam file: .idx suffix */

#define DB_TMP_FL_DIR   TRANS_DIR    /* db tmp file dir */
 
#define SHM_KEY_SYNC  0x10700DAB     /* shared memory key for SYNCDB proc */

#define ENT_SZ_DBF_STATE_TBL   sizeof(unsigned char)
                                 /* size of each entry of db file state table */
                                 /* there are three states for each file:
                                        SYNCED     - file synced
                                        IN_SYNC    - file in the process
                                                     of being synched
                                        NOT_SYNC   - file not synced */
                                  /* total size of DataBase STATE TaBLe is
                                        ENT_SZ_DBF_STATE_TBL * MAX_FILES     */
#define TRANS_WR_CNT_SZ        sizeof (int)
                                  /* transact writes count size in shard mem */
                                  /* used as an indicator to swap transaction
                                     file in a flip-flop fashion */
#define TRANS_FILE_ALT_SZ      sizeof (unsigned char)
                                  /* transact file alternatives in shard mem */
                                  /* used to indicate which file it is writing
                                     to currently */
#define FILE_ALT0 0x01            /* transaction file alternative 0 */
#define FILE_ALT1 0x00            /* transaction file alternative 1 */

#define FLIP_FLOP_LMT          1000      /* limit on no of transaction writes
                                            for transaction file swap */
#define FLIP_WINDOW            10     /* window to start to check flipping
                                         trans file */

#define NOT_SYNCED             0x00      /* not synched or not in sync */
#define SYNCED                 0x10      /* synced */
#define IN_SYNC                0x20      /* in the process of being synched */
#define DONT_CARE              0x40      /* don't care, shadowing any time */

#define IS_SYNCED(addr)        *(unsigned char *)(addr) & SYNCED
#define IS_IN_SYNC(addr)       *(unsigned char *)(addr) & IN_SYNC
#define IS_NOT_SYNCED(addr)    (*(unsigned char *)(addr) | NOT_SYNCED) ? 0 : 1
#define IS_DONT_CARE(addr)     *(unsigned char *)(addr) & DONT_CARE
#define SET_SYNCED(addr)       {*(unsigned char *)(addr) = SYNCED;}
#define SET_IN_SYNC(addr)      {*(unsigned char *)(addr) = IN_SYNC;}
#define SET_NOT_SYNC(addr)     {*(unsigned char *)(addr) = NOT_SYNCED;}
#define SET_DONT_CARE(addr)    {*(unsigned char *)(addr) = DONT_CARE;}

/* mode (flag) Used by init_shm_sync() */

#define CREAT_SHM_INIT         1     /* initial (first time) shared mem init. */
#define SUB_SHM_INIT           0     /* subsquent shared mem access init., i.e.,
                                        init_shm_sync will return attached
                                        shm addr if shm exits, otherwise it is
                                        a failure */
extern int shm_addr;           /* shared memory address of file status table */

/* Flat file IDs */
#define MAX_FLAT_FILES		6

#define	PASSWD_FILE_ID		            0
#define	GROUP_FILE_ID		            1
#define BAD_CREDIT_FILE_ID	            2
#define CREDIT_TRANSACTION_FILE_ID          3
#define RECEIPT_TRANSACTION_FILE_ID         4
#define MISSING_RECEIPT_TRANSACTION_FILE_ID 5

#define TEMP_CREDIT_TRANSACTION_FILE	"/usr/taxi/run/credit_trans"
#define TEMP_RECEIPT_TRANSACTION_FILE "/usr/taxi/run/receipt_trans"
#define TEMP_MISSING_RECEIPT_TRANSACTION_FILE "/usr/taxi/run/missing_receipt_trans"

struct flat_files_struct {
  char *file_name;
  char *temp_file_name;
  int  file_type;
};

#ifdef REDUNDANCY_SRC
struct flat_files_struct flat_files[MAX_FLAT_FILES] = {
  {PASSWD_FILE, TEMP_PASSWD_FILE, PASSWD_FILE_ID},
  {GROUP_FILE, TEMP_GROUP_FILE, GROUP_FILE_ID},
  {BAD_CREDIT_FILE, TEMP_BAD_CREDIT_FILE, BAD_CREDIT_FILE_ID},
  {CREDIT_TRANSACTION_FILE, TEMP_CREDIT_TRANSACTION_FILE, CREDIT_TRANSACTION_FILE_ID},
  {RECEIPT_TRANSACTION_FILE, TEMP_RECEIPT_TRANSACTION_FILE, RECEIPT_TRANSACTION_FILE_ID},
  {MISSING_RECEIPT_TRANSACTION_FILE, TEMP_MISSING_RECEIPT_TRANSACTION_FILE, MISSING_RECEIPT_TRANSACTION_FILE_ID}
};
#else
extern struct flat_files_struct flat_files[MAX_FLAT_FILES];
#endif

  
