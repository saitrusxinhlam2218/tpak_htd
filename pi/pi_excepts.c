static char sz__FILE__[]="@(#)pi_excepts.c	10.1.2.2(ver) Date Release 5/24/95";
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  pi_excepts.c; Rel: 10.1.2.2; Get date: 5/24/95 at 14:43:44
 * @(#)  Last delta: 5/24/95 at 14:43:34
 * @(#)  SCCS File: /taxi/sccs/s.pi_excepts.c
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:ui_excepts.c	6.17.0.0"

/* Based on R3 version 5.20 */

/* Make sure location constants are defined. Create a compile error if they
   are not defined or both are defined. */
#ifdef SCANDINAVIA
#ifdef NAMERICA
ERROR: "Both compile time constants SCANDINAVIA and NAMERICA are defined."
#endif
#else
#ifdef NAMERICA
#else
ERROR: "Compile time constant SCANDINAVIA or NAMERICA undefined."
#endif
#endif

#include <sys/types.h>
#include <memory.h>
#include <ctype.h>
#include <string.h>

#include "mads_types.h"
#include "mads_isam.h"
#include "ex_key.h"
#include "calls.h"
#include "except.h"
#include "mad_ipc.h"
#include "df_maint.h"
#include "call_entry.h"
#include "ui_ipc.h"
#include "mesg.h"
#include "language.h"
#include "pi.h"
#include "semops.h"

#define		VALUE_TO_WAIT_FOR	0	/* for semaphore lock */
#define		PLACE_LOCK		1	/* to place read lock on exceptions structure in shared mem using semop() */
#define		RELEASE_LOCK		-1	/* to release read lock on exceptions structure in shared mem using semop() */
#define		NOT_AN_EXCEPTION	0	/* if exception nbr = 0, its not an exception */
#define		LENGTH_SUM_DESC		133	/* length of sum desc */
#define		MAX_EXCEPT_ZONES	16	/* # of zones/superzones the user can select for exception selection */

/********************************************************************************/
/* ui_excepts.c: contains routines for exceptions processing 			*/
/********************************************************************************/

extern int uid;						/* users id # */
extern char req_buf[];
extern short end_exceptions;				/* flag for ending exceptions.. define in ui_ftnkey() */
extern struct cisam_cl srch_calls[];			/* for storing calls rec from disk */
extern short clear_err_flag;				/* set if last line error should be cleared */
extern struct offsets *offset;				/* pointer to offsets struct in shared memory */
extern int calls_on_page;
char sum_desc[NUM_OF_EXCEPTIONS*LENGTH_SUM_DESC];		/* buffer for summary desc */

extern int fifo_fd;					/* file descriptor for FIFO */
extern int pid;						/* process id */
short resolved_except[NUM_OF_EXCEPTIONS];		/* 1=exception resolved; 0=not resolved */
extern int semid;					/* semaphore id */
short msg_on_display=0;					/* set if a message window is on display */
extern struct except_bits selected_except;		/* for saving default exception selection criteria */
struct except_bits temp_selected_except;		/* temporary exception selection criteria */
extern struct recv_msg *message_read();
extern struct fleets *fleet[];				/* array of pointers to fleets struct in shared memory */
extern char *ui_syserr[];
extern char excpt_fl[];					/* fleets selected for exception processing */
int mesgs();

extern struct 	call_rec 	disp_call;	/* for call rec to FIFO */
extern struct	cisam_cl	call;
int	own_isrecnum;
extern char *Spaces;

/* get_except_call_rec: reads a call record from the database using 
 * the cisam rec # or the call #. If record is read successfully, then
 * 1 is returned, else 0 is returned
 */
int 
get_except_call_rec(walk_except_ptr)
struct excepts *walk_except_ptr;	/* pointer to except slot for which call rec is to be read */
{
	struct cisam_cl *temp_cl_ptr;	/* for req_buf[] */
	int cisam_num;	/* cisam rec # of the call rec */

	/* call_ptr is NULL, try to read record by call # */
	temp_cl_ptr = (struct cisam_cl *)req_buf;
	temp_cl_ptr->nbr = walk_except_ptr->call_nbr;
	if(db(CALL_FILE_ID, READ_KEYS, &cl_key1, ISEQUAL, 0) < 0) 		/* db failed, no record found */
		return(0);

	/* data is saved in req_buf */
	return(1);
}

/*******************************************************************************************************************************/
/* get_excepts: selects 17 (or less) exceptions from the shared memory; reads appropriate records from the disk.               */
/* Returns: # of exceptions found											       */
/********************************************************************************************************************************/
int get_excepts(ptr_ptr, cur_except_type, cur_except_mess, cur_except_num)
struct excepts **ptr_ptr;	/* pointer to the 1st pointer in the array of pointers to the exceptions slots in shared memory */
int cur_except_type;
long cur_except_mess;
long cur_except_num;
{
	int i;					/* scratch variable */
	int got_excepts;			/* # of exceptions found */
	struct excepts *walk_except_ptr;	/* pointer to walk thru exceptions slots */
	int pi;					/* index for except slot pointers */
	int j;					/* scratch variable */

	walk_except_ptr = (struct excepts *)0;			/* initialize pointers, etc */
	
	SetReadLock(semid);                             /* place read lock on the exceptions structure in shared memory */

	walk_except_ptr = offset->first_except;		/* so start from the first exception */
	for (i = 0; i < NUM_OF_EXCEPTIONS; i++ )	/* set pointers to NULL's */
	  *(ptr_ptr + i) = (struct excepts *)0;

	got_excepts = 0;				/* got none so far */
	pi = 0;						/* index for exceptions slot pointers */
	while (got_excepts < NUM_OF_EXCEPTIONS && walk_except_ptr != (struct excepts *)0) 
          { /* loop until all exceptions got (max 10). */
	    if (except_matches(walk_except_ptr, cur_except_type, cur_except_mess, cur_except_num)) 
              {  
                *(ptr_ptr + pi) = walk_except_ptr;				/* assign exception slot pointer */
                got_excepts++;				/* we got an exception, so try to get the next exception */
                pi++;
    	      }    
    	     walk_except_ptr = walk_except_ptr->next;	/* go to next exception slot */
	   }

	if(got_excepts) 					/* if found new exceptions */
	  for (i=pi; i < NUM_OF_EXCEPTIONS; i++)		/* for the remainder of 17 - # of exceptions found */
	    *(ptr_ptr + i) = (struct excepts *)0;	/* set pointers to NULL */

	ReleaseReadLock(semid);                         /* release the semaphore lock */

	for (i = 0; i < got_excepts; i++) {					/* read call records from disk */
		walk_except_ptr = *(ptr_ptr + i);				/* set pointer to exception to be displayed */
		if (walk_except_ptr->call_ptr != (struct calls *)0 || walk_except_ptr->call_nbr > (long)0) {
			if (get_except_call_rec(walk_except_ptr)) {
				memcpy((char *)&srch_calls[i],req_buf,sizeof(struct cisam_cl));
				if (walk_except_ptr->type == REQ_TO_TLK || walk_except_ptr->type == SYS_ERR){
					if (walk_except_ptr->veh_nbr != srch_calls[i].veh_nbr){
						SetReadLock(semid);                                     /* place semaphore lock */
						walk_except_ptr->call_ptr = NULL;			/* not on this call */
						walk_except_ptr->call_nbr = (long) 0;			/* anymore */
						ReleaseReadLock(semid);                                 /* release semaphore lock */
					}
				}
			}
			else {										/* cant read call rec */
				SetReadLock(semid);                                                     /* place semaphore lock */
				walk_except_ptr->call_ptr = NULL;					/* wrong call */
				walk_except_ptr->call_nbr = (long) 0;		
				ReleaseReadLock(semid);                                                 /* release semaphore lock */
			}
		}
	}

    return(got_excepts);
}

/********************************************************************************************************************************/
/* except_matches: checks if the exception slot pointed to by the argumnet pointer conatins an exception which meets the users  */
/* exception criteria. 														*/
/* Returns 0 if criteria not met, 1 if criteria is met										*/
/********************************************************************************************************************************/
except_matches(walk_except_ptr, except_type, except_mess, except_num)
struct excepts *walk_except_ptr;			/* pointer to the exception slot */
int except_type;
long except_mess;
long except_num;
  {
    if ( except_num != 0 )
      {
        if ( walk_except_ptr->nbr != except_num )	/* Wrong exception-number. */
          return(0);
      }
    else
      {
         if ( walk_except_ptr->type != except_type )        /* Wrong type. */ 
           return(0);
	 
#ifdef REST_MESSAGES     
         if (( except_type == DRV_MSG ) || ( except_type == MESSAGE ))
           if ( walk_except_ptr->info != except_mess )		/* Wrong kind of message. */
             return(0);
#endif
      }

    return(1);
  }

/********************************************************************************************************/
/* resolve_except: tries to resolve an exception. Updates the calls file on disk; the call		*/
/*		   history file, and formats and writes a request to disptach. This function		*/
/*		   also set the inuser byte in shared memory						*/
/********************************************************************************************************/

resolve_except(except_ptr,except_index,zv_num,ch_call_flag,ch_hist_flag,resolve_code,action_code,except_type)
struct excepts *except_ptr[];
int except_index;				/* index of the exception highlighted */
int zv_num;					/* zone or vehicle # entered */
short ch_call_flag;				/* 0=do not update call file on disk */
short  ch_hist_flag;				/* 0=do not update call history rec */
int resolve_code;				/* exception resolution code */
char *action_code;				/* action code description */
int except_type;				/* type of exception */
{
	struct excpt_rec except_fifo;		/* for writing except resolution record to FIFO */
	struct cisam_cl *temp_cl_ptr;
	int i,j;
	char temp_comment[41];
	char temp_zone_nbr[4];			/* temporary zone number holder */
	char temp_veh_nbr[5];			/* temporary vehicle number holder */
	char temp_str[7];			/* temporary string holder */
	char exdesc[9];				/* description of exception from shared memory */
	struct excpt_desc *exptr;		/* pointer to exception desc slot */
	char date_str[10];			/* for formatting date */
	char time_str[10];			/* for formatting time */
	char cancel_action;			/* cancel action byte from fleet struct in shared memory; for CANCEL key */
	char call_except_num;			/* '*' = call has more than 1 exception; ' ' = call has 1 exception */
	int	call_nbr;			/* Counter for canceling multiple calls */
	int	call_save;			/* Stores grouped with call for next search */
	int	indx;				/* Index into srch screen array */
	short   back_to_original_call;
	struct	excepts	*ex_ptr;
	char	*last_six();
	int	line_num;
	int	cancelling;

	
	if(except_ptr[except_index]->inuse == '1' || except_ptr[except_index]->nbr == 0) {	/* already resolved ? */
		resolved_except[except_index] = 1;
		return;
	}

        if (resolve_code == (short)R_CANCEL) {                                  /* is user cancelling a call ? */
	        cancelling = 1;
                for (i = 0; i < FLEET_MAX; i++) {
                        if (fleet[i]->fleet_id == srch_calls[except_index].fleet) {
                                cancel_action = fleet[i]->cancel_action;        /* get cancel action for this fleet */
                                        break;
                        }
                }
                if (cancel_action == '3')
                        ch_call_flag = 1;                                       /* update call rec on disk */
                else {
                        ch_call_flag = 0;                                       /* do not update call rec on disk */
                }
        }
	else
	  cancelling = 0;

/*
 *      linked multiple calls
 */
	if(except_type == MULT && srch_calls[except_index].grouped_with) {
                switch (resolve_code) {
                    case R_APPROVE:
			temp_cl_ptr = (struct cisam_cl *)req_buf;
			temp_cl_ptr->nbr = srch_calls[except_index].nbr;		/* Get call # to read in */

			if((db(CALL_FILE_ID, READ_KEYS, &cl_key1, ISEQUAL, 0)) < 0)  {	/* read in call */
				return;							/* Get out */
			}

			memcpy(&call, req_buf, sizeof(struct cisam_cl));
			own_isrecnum = isrecnum;
			fill_call_rec();
			if(write(fifo_fd, &disp_call, sizeof(struct call_ipc_rec)) == -1) { 	/* write on fifo failed */
				return;
			}

			if(read_callh_rec(srch_calls[except_index].nbr, MULT)) 
 				update_callh_rec("GODK[ND", srch_calls[except_index].pckup_zone);	/* update hist rec */

			except_fifo.u_prcs_id = pid;
			except_fifo.rec_type = EXCEPT_REQ;
			except_fifo.veh_nbr = 0;
			except_fifo.zone_nbr = 0;	
			except_fifo.old_zone_nbr = 0;	
			except_fifo.old_due_time = 0;	

			call_nbr = srch_calls[except_index].grouped_with;

			except_fifo.resolve_code = (short)R_MULT;

			while(call_nbr != srch_calls[except_index].nbr) {
				ex_ptr = offset->first_except;
				indx = 0;
				while(ex_ptr != NULL) {
					if(call_nbr == ex_ptr->call_nbr)
						break;
/*
 *                              fix for highlighting correct multiple call exceptions
 */
                                        if (except_matches(ex_ptr))
                                                indx++;
                                        ex_ptr = ex_ptr->next;
                                }

				except_fifo.call_nbr = call_nbr;

 				if(read_callh_rec(call_nbr, MULT)) 
 					update_callh_rec("GODK[ND", srch_calls[except_index].pckup_zone);  /* update hist rec */

				if(ex_ptr != NULL) {
					ex_ptr->inuse = '1';
					/* indx only points into the exception list. If there are previously
					   cancelled calls, they will still be on the screen in srch_calls,
					   but no longer in the exception list. Find the place in the srch_calls: */
					line_num = -1;
					for (i = 0; i < NUM_OF_EXCEPTIONS; i ++)
					  if (srch_calls[i].nbr == ex_ptr->call_nbr)
					    {
					      line_num = i;
					      break;
					    }
					if (line_num >= 0)
					  resolved_except[line_num] = 1;

					except_fifo.except_ptr = ex_ptr;
					except_fifo.except_nbr = ex_ptr->nbr;
				}

				temp_cl_ptr = (struct cisam_cl *)req_buf;
				temp_cl_ptr->nbr = call_nbr;
				if(db(CALL_FILE_ID, READ_KEYS, &cl_key1, ISEQUAL, 0) < 0) {
					return;
				}

				call_nbr = except_fifo.grouped_with = temp_cl_ptr->grouped_with;

				if(write(fifo_fd, &except_fifo, sizeof(struct excpt_rec)) == -1)
				  ERROR("Cannot write to FIFO.");

			}

			except_ptr[except_index]->inuse = '1';
			resolved_except[except_index] = 1;

/*
 *                      the highlighted call gets sent to dispatch with a resolve code of approve
 *                              so that it gets put into shared memory
 *                              all other members of the group get sent with R_MULT so that they
 *                              don't get put into shared memory
 */
			except_fifo.resolve_code = (short)resolve_code;

			except_fifo.except_ptr = except_ptr[except_index];
			except_fifo.except_nbr = except_ptr[except_index]->nbr;
			except_fifo.call_nbr = srch_calls[except_index].nbr;
			except_fifo.grouped_with = srch_calls[except_index].grouped_with;
			if(write(fifo_fd, &except_fifo, sizeof(struct excpt_rec)) == -1)
				ERROR("Cannot write to FIFO.");

			return;
			break;

                    case R_DISSAPROVE:
			call_nbr = srch_calls[except_index].nbr;

			while(call_nbr != 0) {
				for(indx = 0; indx < 17; indx++)
					if(call_nbr == srch_calls[indx].nbr)
						srch_calls[indx].grouped_with = 0;
/*
 *                              fix for multiple call
 *                              call history not updated on unlink of calls
 
 				if(read_callh_rec(call_nbr, MULT)) 
 					update_callh_rec(ACT_DISAPPROVE, 0);		/* update hist rec *
 */

				temp_cl_ptr = (struct cisam_cl *)req_buf;
				temp_cl_ptr->nbr = call_nbr;
				if(db(CALL_FILE_ID, READ_KEYS, &cl_key1, ISEQUAL+ISLOCK, 0) < 0) {
				  ERROR("Cannot read record - CALL-FILE");	
					return;
				}

				call_save = temp_cl_ptr->grouped_with;
				temp_cl_ptr->grouped_with = 0L;

				if(db(CALL_FILE_ID,UPDATE_REC,&cl_key1,0,0) < 0) {
				  ERROR("Cannot update record - CALL-FILE");	
					return;
				}

				if(db(CALL_FILE_ID,RELEASE,&cl_key1,0,0) < 0)
				  ERROR("Cannot release record - CALL-FILE");	

				call_nbr = call_save;
				if(call_nbr == srch_calls[except_index].nbr)
					call_nbr = 0L;
			}
			return;
			break;

                    default:
                        break;
                  }
          }

/*
 *      unlinked multiple call
 */
/*
 *      fix for unlinked multiple call APPROVE
 *              error, multiple call must be linked for approve
 */
        if (except_ptr[except_index]->type == MULT &&
            !srch_calls[except_index].grouped_with &&
            !strcmp(action_code, "GODK[ND"))
          {
                return;
          }
/*
 *      non-multiple call processing
 */

	except_ptr[except_index]->inuse = '1';					/* we can resolve this exception */
	resolved_except[except_index] = 1;
	
	if (except_ptr[except_index]->type == REQ_TO_TLK && except_ptr[except_index]->call_ptr == NULL && 
	    except_ptr[except_index]->call_nbr == NULL)	/* if REQ_TO_TALK excep; no need to add/read call history record; */
		ch_hist_flag = 0;				/*  so set flag to 0 */

	if(ch_call_flag && resolve_code != (short)R_CANCEL && resolve_code != R_M_MAN_VEH) {  /* update call record on disk */
		if (resolve_code == (short)R_MAN_VEH) {		/* MAN VEH assignment ? */
			srch_calls[except_index].veh_nbr = (short)zv_num;
			if (zv_num > 0) {
				strcpy(srch_calls[except_index].status,ASSIGNED);
				srch_calls[except_index].pri_status = 64;		/* call is now assigned */
				srch_calls[except_index].dispd_date_time = (int)time(NULL);
				get_pkup_date(date_str);
				get_pkup_time(time_str);
				strcpy(srch_calls[except_index].dispd_date,date_str);
				strcpy(srch_calls[except_index].dispd_time,time_str);
				srch_calls[except_index].dispatched_by = (short)uid;
			}
			else {
				strcpy(srch_calls[except_index].status,"EJ TILLD");	/* unassignment */
				srch_calls[except_index].pri_status = srch_calls[except_index].priority;
				srch_calls[except_index].dispd_date_time = NULL;
				srch_calls[except_index].dispd_date[0] = NULL;
				srch_calls[except_index].dispd_time[0] = NULL;
				srch_calls[except_index].dispatched_by = 0;
				srch_calls[except_index].pckup_date_time = NULL;
				srch_calls[except_index].pckup_time[0] = NULL;
				srch_calls[except_index].pckup_date[0] = NULL;
				srch_calls[except_index].close_date_time = NULL;
				srch_calls[except_index].close_time[0] = NULL;
				srch_calls[except_index].close_date[0] = NULL;
				srch_calls[except_index].veh_nbr = 0;
				srch_calls[except_index].drv_id = 0;
			}
		}
		else if (resolve_code == (short)R_DISPATCH) {		/* this is an action, to dispach to starat the call again */
			strcpy(srch_calls[except_index].status,"EJ TILLD");
			srch_calls[except_index].pri_status = srch_calls[except_index].priority;
			srch_calls[except_index].dispd_date_time = NULL;
			srch_calls[except_index].dispd_time[0] = NULL;
			srch_calls[except_index].dispd_date[0] = NULL;
			srch_calls[except_index].pckup_date_time = NULL;
			srch_calls[except_index].pckup_time[0] = NULL;
			srch_calls[except_index].pckup_date[0] = NULL;
			srch_calls[except_index].close_date_time = NULL;
			srch_calls[except_index].close_time[0] = NULL;
			srch_calls[except_index].close_date[0] = NULL;
			srch_calls[except_index].veh_nbr = 0;
			srch_calls[except_index].dispatched_by = 0;
			srch_calls[except_index].drv_id = 0;
		}
		else {				/* depending upon the type of exception, update the call record accordingly */
			switch (except_type)  {
				case UNZONED:						/* update this record with new zone # */
					if (srch_calls[except_index].pckup_zone <= 0)
						update_unzoned(srch_calls[except_index].fleet, srch_calls[except_index].nbr,
							       zv_num, uid);
					srch_calls[except_index].pckup_zone = zv_num;
					srch_calls[except_index].last_changed_by = uid;
					break;

								/* update status to completed, add priority = 64, and add the */
				case WAKE_UP:			/* various times ... for APPROVE key entered */
					strcpy(srch_calls[except_index].status,COMPLETE);
					srch_calls[except_index].pri_status = (short)64;
					srch_calls[except_index].dispd_date_time = (int)time(NULL);
					srch_calls[except_index].date_time_in = srch_calls[except_index].dispd_date_time;
					srch_calls[except_index].pckup_date_time = srch_calls[except_index].dispd_date_time;
					srch_calls[except_index].close_date_time = srch_calls[except_index].dispd_date_time;
					get_pkup_date(date_str);
					get_pkup_time(time_str);
					strcpy(srch_calls[except_index].dispd_date,date_str);
					strcpy(srch_calls[except_index].dispd_time,time_str);
					strcpy(srch_calls[except_index].pckup_date,date_str);
					strcpy(srch_calls[except_index].pckup_time,time_str);
					strcpy(srch_calls[except_index].close_date,date_str);
					strcpy(srch_calls[except_index].close_time,time_str);
					break;

/*
 *                              fix for disapproving unlinked multiple call
 *                                      set extended to not multiple
 */
                                case MULT:
                                        set_multiple_call_type(srch_calls[except_index].extended_type, 0);
                                        break;

				case CALL_NO_MSG:
				case CALL_W_MSG:
				case HOL_TM_CALL:
				case EXTRA_TAXIS:
					if (resolve_code == R_DISSAPROVE){
						strcpy(srch_calls[except_index].status,"AVBEST  ");
						srch_calls[except_index].pri_status = (short)64;
					}
					break;

				case CALL_CANCEL:		/* for approve key */
					if (resolve_code == R_APPROVE){
						strcpy(srch_calls[except_index].status,"AVBEST  ");
						srch_calls[except_index].pri_status = (short)64;
						call_save = srch_calls[except_index].grouped_with;
						srch_calls[except_index].grouped_with = 0;
					}
					break;
				
				default:
				break;
		
			}

		}

		temp_cl_ptr = (struct cisam_cl *)req_buf;
		temp_cl_ptr->nbr = srch_calls[except_index].nbr;
		if(db(CALL_FILE_ID,READ_KEYS,&cl_key1,ISEQUAL+ISLOCK,0) < 0) {			/* try to read rec by call # */
			ERROR("Cannot read record - CALL-FILE");	
			except_ptr[except_index]->inuse = '0';
			resolved_except[except_index] = 0;
			return;
		}

		memcpy(req_buf,(char *)&srch_calls[except_index],sizeof(struct cisam_cl));	/* make changes on disk record */
		if (db(CALL_FILE_ID,UPDATE_REC,&cl_key1,0,0) < 0) {
			ERROR("Cannot update record - CALL-FILE");	
			except_ptr[except_index]->inuse = '0';
			resolved_except[except_index] = 0;
			return;
		}

		if(db(CALL_FILE_ID,RELEASE,&cl_key1,0,0) < 0)
			ERROR("Cannot release record - CALL-FILE");	
	}

/*
 *      multiple call vehicle assignment
 */

	if(resolve_code == R_M_MAN_VEH) {		/* MAN VEH assignment ? */
		call_nbr = srch_calls[except_index].nbr;

		except_fifo.except_ptr = except_ptr[except_index];
		except_fifo.except_nbr = except_ptr[except_index]->nbr;

		while(call_nbr != 0) {
			for(indx = 0; indx < 17; indx++) {
				if(call_nbr == srch_calls[indx].nbr)
					break;
			}

/*
 *                      if call is on screen ==> it is in memory
 */

			if(indx < 17) {
				srch_calls[except_index].veh_nbr = (short)zv_num;

				if(zv_num > 0) {
					strcpy(srch_calls[indx].status,ASSIGNED);
					srch_calls[indx].pri_status = 64;		/* call is now assigned */
					srch_calls[indx].dispd_date_time = (int)time(NULL);
					get_pkup_date(date_str);
					get_pkup_time(time_str);
					strcpy(srch_calls[indx].dispd_date,date_str);
					strcpy(srch_calls[indx].dispd_time,time_str);
					srch_calls[indx].dispatched_by = (short)uid;
				}
				else {
					strcpy(srch_calls[indx].status,"EJ TILLD");	/* unassignment */
					srch_calls[indx].pri_status = srch_calls[indx].priority;
					srch_calls[indx].dispd_date_time = NULL;
					srch_calls[indx].dispd_date[0] = NULL;
					srch_calls[indx].dispd_time[0] = NULL;
					srch_calls[indx].dispatched_by = 0;
					srch_calls[indx].pckup_date_time = NULL;
					srch_calls[indx].pckup_time[0] = NULL;
					srch_calls[indx].pckup_date[0] = NULL;
					srch_calls[indx].close_date_time = NULL;
					srch_calls[indx].close_time[0] = NULL;
					srch_calls[indx].close_date[0] = NULL;
					srch_calls[indx].veh_nbr = 0;
					srch_calls[indx].drv_id = 0;
				} 
			}

/*
 *                      call is not on screen ==> bring it into memory
 */

			except_fifo.u_prcs_id = pid;							/* write to dispatch */
			except_fifo.rec_type = EXCEPT_REQ;
			except_fifo.resolve_code = (short)resolve_code;
			except_fifo.grouped_with = 0;
			except_fifo.veh_nbr = (short)zv_num;
			except_fifo.zone_nbr = 0;	
			except_fifo.old_zone_nbr = 0;	
			except_fifo.old_due_time = 0;	

			temp_cl_ptr = (struct cisam_cl *)req_buf;
			temp_cl_ptr->nbr = call_nbr;
			if(db(CALL_FILE_ID, READ_KEYS, &cl_key1, ISEQUAL+ISLOCK, 0) < 0) {	/* try to read rec by call # */
				ERROR("Cannot read record - CALL-FILE");	
				except_ptr[except_index]->inuse = '0';
				resolved_except[except_index] = 0;
				return;
			}

			except_fifo.call_nbr = call_nbr;			/* exception is not associated with a call */
			call_nbr = temp_cl_ptr->grouped_with;

			temp_cl_ptr->veh_nbr = (short)zv_num;

			if(zv_num > 0) {
				strcpy(temp_cl_ptr->status,ASSIGNED);
				temp_cl_ptr->pri_status = 64;		/* call is now assigned */
				temp_cl_ptr->dispd_date_time = (int)time(NULL);
				get_pkup_date(date_str);
				get_pkup_time(time_str);
				strcpy(temp_cl_ptr->dispd_date,date_str);
				strcpy(temp_cl_ptr->dispd_time,time_str);
				temp_cl_ptr->dispatched_by = (short)uid;
			}
			else {
				strcpy(temp_cl_ptr->status,"EJ TILLD");	/* unassignment */
				temp_cl_ptr->pri_status = temp_cl_ptr->priority;
				temp_cl_ptr->dispd_date_time = NULL;
				temp_cl_ptr->dispd_date[0] = NULL;
				temp_cl_ptr->dispd_time[0] = NULL;
				temp_cl_ptr->dispatched_by = 0;
				temp_cl_ptr->pckup_date_time = NULL;
				temp_cl_ptr->pckup_time[0] = NULL;
				temp_cl_ptr->pckup_date[0] = NULL;
				temp_cl_ptr->close_date_time = NULL;
				temp_cl_ptr->close_time[0] = NULL;
				temp_cl_ptr->close_date[0] = NULL;
				temp_cl_ptr->veh_nbr = 0;
				temp_cl_ptr->drv_id = 0;
			} 

			if(db(CALL_FILE_ID,UPDATE_REC,&cl_key1,0,0) < 0) {
				ERROR("Cannot update record - CALL-FILE");	
				except_ptr[except_index]->inuse = '0';
				resolved_except[except_index] = 0;
				return;
			}

			if(db(CALL_FILE_ID,RELEASE,&cl_key1,0,0) < 0)
				ERROR("Cannot release record - CALL-FILE");	

                        if(read_callh_rec(call_nbr, MULT))
                                update_callh_rec(action_code, srch_calls[except_index].pckup_zone);  /* update hist rec */

			if(write(fifo_fd, &except_fifo, sizeof(struct excpt_rec)) == -1) 	/* write to FIFO */
				ERROR("<except rec to FIFO>");

			except_fifo.except_ptr = NULL;
			except_fifo.except_nbr = 0;

			if(call_nbr == srch_calls[except_index].nbr) 				/* Don't do first call again */
				call_nbr = 0L;
 
                        ERROR("<Multiple Call>");
		}
		goto done;
	}

/*
 *      approving a linked multiple call CANCEL
 */
	/* APPROVE CANCEL MULT CALL or cancel call if cancel_action='3' (for automatic) */
	if(resolve_code == R_CAN_MULT || (resolve_code == R_CANCEL && cancel_action == '3')) {
		call_nbr = srch_calls[except_index].nbr;

		except_fifo.except_ptr = except_ptr[except_index];
		except_fifo.except_nbr = except_ptr[except_index]->nbr;

		while(call_nbr != 0) {
			ex_ptr = offset->first_except;
			indx = 0;
			while(ex_ptr != NULL) {
				if(call_nbr == ex_ptr->call_nbr)
					break;
/*
 *                      fix for highlighting correct multiple call exceptions
 */
                                if (except_matches(ex_ptr))
					indx++;
				ex_ptr = ex_ptr->next;
			}

			if(ex_ptr != NULL) {
				ex_ptr->inuse = '1';
				/* indx only points into the exception list. If there are previously
				   cancelled calls, they will still be on the screen in srch_calls,
				   but no longer in the exception list. Find the place in the srch_calls: */
				line_num = -1;
				for (i = 0; i < NUM_OF_EXCEPTIONS; i ++)
				  if (srch_calls[i].nbr == ex_ptr->call_nbr)
				    {
				      line_num = i;
				      break;
				    }
				if (line_num >= 0)
				  resolved_except[line_num] = 1;
			}

/*
 *                      fix for updating call history on a mult cancel
 *                      approve the cancel
 */
			/* update hist rec */

			/* For mult cancel */
                        if (resolve_code == R_CAN_MULT)
			  {
			    if (read_callh_rec(call_nbr, CALL_CANCEL))
			      update_callh_rec("GODK[ND", srch_calls[except_index].pckup_zone);
			  }
			/* For normal cancel */
			else
			  add_noexcpt_callh(except_index, resolve_code, action_code, zv_num);

			if(line_num >= 0) {
				strcpy(srch_calls[line_num].status, "AVBEST  ");
				srch_calls[line_num].pri_status = (short)64;
				srch_calls[line_num].grouped_with = 0;
			}

			except_fifo.u_prcs_id = pid;							/* write to dispatch */
			except_fifo.rec_type = EXCEPT_REQ;
			except_fifo.resolve_code = (short)resolve_code;
			except_fifo.grouped_with = 0;
			except_fifo.veh_nbr = 0;
			except_fifo.zone_nbr = 0;	
			except_fifo.old_zone_nbr = 0;	
			except_fifo.old_due_time = 0;	
			except_fifo.call_nbr = call_nbr;

			temp_cl_ptr = (struct cisam_cl *)req_buf;
			temp_cl_ptr->nbr = call_nbr;
			if(db(CALL_FILE_ID, READ_KEYS, &cl_key1, ISEQUAL+ISLOCK, 0) < 0) {	/* try to read rec by call # */
				ERROR("no record found"); 
				except_ptr[except_index]->inuse = '0';
				resolved_except[except_index] = 0;
				return;
			}

			call_nbr = temp_cl_ptr->grouped_with;
			temp_cl_ptr->grouped_with = 0L;
			strcpy(temp_cl_ptr->status, "AVBEST  ");
			temp_cl_ptr->pri_status = (short)64;

			if(db(CALL_FILE_ID,UPDATE_REC,&cl_key1,0,0) < 0) {
				ERROR("<error updating calls record... FATAL ERROR>");		/* db failed */
				except_ptr[except_index]->inuse = '0';
				resolved_except[except_index] = 0;
				return;
			}

			if(db(CALL_FILE_ID,RELEASE,&cl_key1,0,0) < 0)
				ERROR("<error releasing lock on calls record>");	/* db failed */

			if(write(fifo_fd, &except_fifo, sizeof(struct excpt_rec)) == -1) 	/* write to FIFO */
				ERROR("<except rec to FIFO>");

			except_fifo.except_ptr = NULL;
			except_fifo.except_nbr = 0;

			if(call_nbr == srch_calls[except_index].nbr) 				/* Don't do first call again */
				call_nbr = 0L;
		}
		goto done;
	}

/*
 *      disapproving a linked multiple call CANCEL
 */
        if (except_type == CALL_CANCEL &&
            resolve_code == R_DISSAPROVE &&
            srch_calls[except_index].grouped_with)
 
          {
                except_fifo.u_prcs_id = pid;
                except_fifo.rec_type = EXCEPT_REQ;
                except_fifo.veh_nbr = 0;
                except_fifo.zone_nbr = 0;
		except_fifo.old_zone_nbr = 0;	
		except_fifo.old_due_time = 0;	
 
                back_to_original_call = 0;
                call_nbr = srch_calls[except_index].grouped_with;
                while (!back_to_original_call)
                  {
                        ex_ptr = offset->first_except;
                        indx = 0;
                        while(ex_ptr != NULL) {
                                if(call_nbr == ex_ptr->call_nbr)
                                        break;
                                if (except_matches(ex_ptr))
                                        indx++;
                                ex_ptr = ex_ptr->next;
                        }


                        if(read_callh_rec(call_nbr,except_ptr[except_index]->type))
                                update_callh_rec(action_code, 0);

                        if(ex_ptr != NULL)
                          {
                                ex_ptr->inuse = '1';
				/* indx only points into the exception list. If there are previously
				   cancelled calls, they will still be on the screen in srch_calls,
				   but no longer in the exception list. Find the place in the srch_calls: */
				line_num = -1;
				for (i = 0; i < NUM_OF_EXCEPTIONS; i ++)
				  if (srch_calls[i].nbr == ex_ptr->call_nbr)
				    {
				      line_num = i;
				      break;
				    }
				if (line_num >= 0)
				  resolved_except[line_num] = 1;

                                except_fifo.except_ptr = ex_ptr;
                                except_fifo.except_nbr = ex_ptr->nbr;
                          }

                        temp_cl_ptr = (struct cisam_cl *)req_buf;
                        temp_cl_ptr->nbr = call_nbr;
                        if(db(CALL_FILE_ID, READ_KEYS, &cl_key1, ISEQUAL, 0) < 0)
                          {
                                ERROR("<calls file .... FATAL ERROR>");
                                return;
                          }

                        if (call_nbr == srch_calls[except_index].nbr)
                                back_to_original_call = 1;

                        call_nbr = except_fifo.grouped_with = temp_cl_ptr->grouped_with;

                        except_fifo.resolve_code = (short)resolve_code;
                        except_fifo.call_nbr = temp_cl_ptr->nbr;

                        if(write(fifo_fd, &except_fifo, sizeof(struct excpt_rec)) == -1)
                                ERROR("<except rec to FIFO>");
                  }

                goto done;
          }



	if (ch_hist_flag) {						/* updat the call history file for this exception */
		if(resolve_code ==R_CALLBACK || resolve_code ==R_RESEND || resolve_code ==R_DISPATCH || resolve_code ==R_CANCEL ||
		   (resolve_code == R_SUP_HOLD && except_ptr[except_index]->call_ptr != (struct calls *)0 &&
		   except_ptr[except_index]->call_ptr->status.sup_hold == 0)) {
			add_noexcpt_callh(except_index,resolve_code,action_code,zv_num);
		}
 		else if (resolve_code == R_SUP_HOLD  && except_ptr[except_index]->call_ptr != (struct calls *)0 &&
 			except_ptr[except_index]->call_ptr->status.sup_hold == 1) {		/* already on HOLD ? */
 			if(read_callh_rec(srch_calls[except_index].nbr,SUP_HOLD)) 
 				update_callh_rec(action_code,0);				/* take it off HOLD */
 		}
		else if(read_callh_rec(srch_calls[except_index].nbr,except_ptr[except_index]->type)) {	/* got rec from disk */
			switch (resolve_code)
			{
				case R_MAN_VEH:
				case R_M_MAN_VEH:
				case R_MAN_ZONE:
				update_callh_rec(action_code,zv_num);			/* should update veh # in call hist rec */
				break;
			default:
				update_callh_rec(action_code,0);
				break;
			}
				
			if(srch_calls[except_index].grouped_with && except_ptr[except_index]->type != NO_SHOW) {
				call_nbr = srch_calls[except_index].grouped_with;

				while(call_nbr != srch_calls[except_index].nbr) {	
					temp_cl_ptr = (struct cisam_cl *)req_buf;
					temp_cl_ptr->nbr = call_nbr;
					if(db(CALL_FILE_ID, READ_KEYS, &cl_key1, ISEQUAL+ISLOCK, 0) < 0) {
						ERROR("<calls file .... FATAL ERROR>");
						return;
					}

					call_save = temp_cl_ptr->grouped_with;

					read_callh_rec(call_nbr, except_ptr[except_index]->type);
					update_callh_rec(action_code, 0);

					if(db(CALL_FILE_ID,RELEASE,&cl_key1,0,0) < 0)
						ERROR("<error releasing lock on calls record>");

					call_nbr = call_save;
				}
			}
		}
	}

							/* for emergency exception, need to update the vehicle record on disk; */
	if (except_type == EMERGENCY) {			/* also upadte the emergency log file and the driver file */
		if (update_veh_rec(except_ptr,except_index) == -1) {				/* cannot update vehicle rec */
			ERROR("<error updating vehicle record... exception resolved>");		/* db failed */
		}
	}
	
	if (except_type == DRV_MSG) {		/* for DRV_MSG ( driver_msg) exception, need to update the messagae log file */
		if (msg_on_display) {	 			/* check if message window should be displayed or erased */
			msg_on_display = 0; 			/* there is a message window on display */
		}
		if (resolve_code == (short)R_APPROVE)  
			message_apprvd_deny((int)except_ptr[except_index]->info, SUPER_APPROVED);
		else 
			message_apprvd_deny((int)except_ptr[except_index]->info, SUPER_DENIED);	/* disapproved */
	}

	except_fifo.u_prcs_id = pid;							/* write to dispatch */
	except_fifo.rec_type = EXCEPT_REQ;
	except_fifo.except_ptr = except_ptr[except_index];
	except_fifo.resolve_code = (short) resolve_code;
	except_fifo.except_nbr = except_ptr[except_index]->nbr;

	if (except_ptr[except_index]->call_ptr != (struct calls *)0 || except_ptr[except_index]->call_nbr > (long)0) {
		except_fifo.call_nbr = srch_calls[except_index].nbr;		/* exception is associated with a call */
		except_fifo.grouped_with = srch_calls[except_index].grouped_with;
	}
	else {
		except_fifo.call_nbr = 0;					/* exception is not associated with a call */
		except_fifo.grouped_with = 0;
	}

	if (resolve_code == (short)R_MAN_VEH)					/* set the vehicle # */
		except_fifo.veh_nbr = zv_num;					/* its manual veh assignment */
	else 
		except_fifo.veh_nbr = 0;

	if (resolve_code == (short)R_MAN_ZONE)					/* set the zone # */
		except_fifo.zone_nbr = zv_num;
	else 
		except_fifo.zone_nbr = 0;	

	except_fifo.old_zone_nbr = 0;	
	except_fifo.old_due_time = 0;	

	if(write(fifo_fd, &except_fifo, sizeof(struct excpt_rec)) == -1) 	/* write to FIFO */
		ERROR("<except rec to FIFO>");

done:
	i = except_index;
	if (except_ptr[except_index]->type != ETA && except_ptr[except_index]->type != MESSAGE &&
	    except_ptr[except_index]->call_ptr != (struct calls *)0 ) { 
		strncpy(temp_comment,srch_calls[i].pckup_adr_cmnt,20);
		temp_comment[20] = '\0';
		exptr = offset->except_desc + except_ptr[i]->type - 1;

		/* Cancelled is a special case */
		if (cancelling)
		  strcpy(exdesc, "AVBEST  ");
		else
		  strcpy(exdesc, exptr->short_desc);

		if (srch_calls[i].pckup_zone)
			sprintf(temp_zone_nbr,"%-03d",srch_calls[i].pckup_zone);
		else
			sprintf(temp_zone_nbr,"   ");

		if (srch_calls[i].veh_nbr)
			sprintf(temp_veh_nbr,"%-03d",srch_calls[i].veh_nbr);
		else
			sprintf(temp_veh_nbr,"   ");

		/* If call has more than one exception, or call has been cancelled and the fleet
		   does not do auto cancelling (cancel_action != '3') and the extra exception has
		   not yet appeared in the status bits (but it will as soon as dispatch processes
		   the message from the FIFO) then put a '*' */
		if ( call_with_more_excepts(except_ptr[i]) || (cancelling && cancel_action != '3'))
			call_except_num = '*';	/* call has more than 1 exception */
		else 
			call_except_num = ' ';	/* call has 1 exception */

	}

	return(1);
}

/********************************************************************************************************/
/* update_veh_rec: updates the vehicle file for emergency exceptions; by setting the			*/
/*		last_emerg_off time to the time the exception is resolved				*/
/********************************************************************************************************/
int update_veh_rec(except_ptr,except_index)
struct excepts *except_ptr[];
int except_index;
{
	struct cisam_vh *vh_ptr;	

	vh_ptr = (struct cisam_vh *)req_buf;
	vh_ptr->fleet = except_ptr[except_index]->fl_ptr->fleet_id;
	vh_ptr->nbr = (short)except_ptr[except_index]->veh_nbr;

	if(db(VEHICLE_FILE_ID, READ_KEYS, &veh_key2, ISEQUAL+ISLOCK, 0) < 0) {
		ERROR("<Invalid vehicle number>");
		return;
	}

	/* update emergency time off field in the vehicle file */
	vh_ptr->last_emerg_off = (int)time(NULL);

	if(db(VEHICLE_FILE_ID, UPDATE_REC, &veh_key2, 0, 0) < 0) {
		ERROR("<error updating vehicle record>");
		return;
	}

	if(db(VEHICLE_FILE_ID,RELEASE,&veh_key2,0,0) < 0) 
		ERROR("<error releasing lock on vehicle record>");
	
	/* update emergency log file and the driver file */
	writer_emer_off(vh_ptr->fleet, vh_ptr->last_emerg_on, (short)uid, vh_ptr->last_emerg_off, except_index);
}

/* call_with_more_excepts: for an exception associated with a call, it determines if the call has
 * more than 1 exception associated with it. 
 *	Returns		0	if call does not have more than 1 exception
 *	Returns		1	if call has more than 1 exception associated with it
 */
int 
call_with_more_excepts(ptr_to_except)
struct excepts *ptr_to_except;	/* pointer to exception slot in shared memory */
{
	int except_count = 0;	/* for determining # of exception with this call */
	int i;

	if(ptr_to_except->call_ptr == (struct calls *)0)
		return(0);	/* this exception is not with a call */

	for(i = 0; i < MANUAL; i++)  {
		if(test_bit(i,sizeof(struct except_bits),(unsigned char *)&ptr_to_except->call_ptr->status)) {
			except_count++;
			if(except_count > 1) 
				return(1);	/* call has more than 1 exception */
		}
	}
	return(0);	/* call has only 1 exception */
}

int test_bit(bit_num,size,struct_ptr)
	int bit_num;	/* # of the bit to be tested; leaset significant = 0 */
	int size;	/* size of bit struct in bytes */
	unsigned char *struct_ptr;	/* pointer to the bit struct */
{
	unsigned char *bit_ptr;	/* poinetr to bit struct */
	unsigned char mask;
	int shift_by;	/* shift the mask by */

	bit_ptr = (unsigned char *)struct_ptr + bit_num/8;
	mask = 0x01;	/* start from bit 0 */
	shift_by = bit_num % 8;
	mask <<= shift_by;

	if ( *bit_ptr & mask )
		/* bit is set */
		return(1);
	else 
		/* bit is not set */
		return(0);

}  /* end test_bit() */

/* set_bit: sets the bit specified by bit_num to 1. The size of the bit structure and its address 
 *		should be passed as arguments
 */
set_bit(bit_num,size,struct_ptr)
	int bit_num;	/* # of the bit to be tested; leaset significant = 0 */
	int size;	/* size of bit struct in bytes */
	unsigned char *struct_ptr;	/* pointer to the bit struct */
{
	unsigned char *bit_ptr;	/* poinetr to bit struct */
	unsigned char mask;
	int shift_by;	/* shift the mask by */

	mask = 0x80;
	bit_ptr = (unsigned char *)struct_ptr + bit_num/8;
	shift_by = bit_num % 8;
	mask >>= shift_by;

	*bit_ptr = *bit_ptr | mask;

}  /* end set_bit() */

/*-------------------------------------------------------------------
 *      set multiple call type
 *              sets extended type to multiple 'M'
 *-------------------------------------------------------------------
 */

set_multiple_call_type(extended_type_field, on)
        char    extended_type_field[];
        short   on;
{
        int     i;
        short   found_null_terminator;
 
        if (!on)
                extended_type_field[4] = '\0';
	else {
	        found_null_terminator = FALSE;
        	for (i=0; i < 4; i++) {
                	if (extended_type_field[i] == '\0' || found_null_terminator) {
                        	extended_type_field[i] = ' ';
                        	found_null_terminator = TRUE;
                  	}
          	}

        	extended_type_field[4] = MULT_CALL;
	}

        extended_type_field[5] = '\0';

        return(1);
}

/************************************************************************************************************************/
/* writer_emer_off - Called by USER_INTERFACE when a driver presses the emergency (M) key.  				*/
/* It updates DRIVER record with the time of the emergency.  								*/
/* It updates the emergency record that was created by DISPATCH when the emergency exception was generated.		*/
/************************************************************************************************************************/
writer_emer_off(fleet, date_time, by, resolve_time, except_index)
char fleet;					/* fleet in which exception is present */
long date_time;					/* date/time the exception was generated */
short by;					/* id of user who resolved the exception */
long resolve_time;				/* time exception is resolved */
int except_index;
{
	struct cisam_dr *dr_ptr;
	struct cisam_em *em_ptr;
	char cur_dt[10];
	char cur_tm[10];
	char comment[79];
	int  dummy1, dummy2;
	int  driver_id;
	
	if (db(EMERG_FILE_ID,OPEN,&em_key1,ISINOUT+ISMANULOCK,0) < 0) {		/* open EMERG file */
		db(EMERG_FILE_ID,CLOSE,&em_key1,0,0);				/* close EMERG file */
		return(-1);							/* if error, return */
	}

	em_ptr = (struct cisam_em *)req_buf;
	bzero(em_ptr, sizeof(struct cisam_em));

	em_ptr->fleet = fleet;
	em_ptr->date_time = date_time;

	if (db(EMERG_FILE_ID,READ_KEYS,&em_key1,ISEQUAL+ISLOCK,0) < 0) {	/* read EMERG record */
	        ERROR("Error reading Emergency file");
		db(EMERG_FILE_ID,RELEASE,&em_key1,0,0);				/* unlock EMERG record */
		db(EMERG_FILE_ID,CLOSE,&em_key1,0,0);				/* close EMERG file */
		return(-1);							/* if error, return */
	}

	get_asc_time(resolve_time, cur_dt, cur_tm);				/* get current time */
	strcpy(em_ptr->resolved_date,cur_dt);
	strcpy(em_ptr->resolved_time,cur_tm);
	em_ptr->resolved_dt_tm = resolve_time;
	em_ptr->resolved_by = by;
	strcpy(em_ptr->comment, comment);

	/* Save since it will be wiped out being stored in req_buf */
	driver_id = em_ptr->drv_id;

	if (db(EMERG_FILE_ID,UPDATE_REC,&em_key1,0,0) < 0) {			/* update EMERG record */
		db(EMERG_FILE_ID,RELEASE,&em_key1,0,0);				/* unlock EMERG record */
		db(EMERG_FILE_ID,CLOSE,&em_key1,0,0);				/* close EMERG file */
		return(-1);							/* if error, return */
	}

	if (db(EMERG_FILE_ID,RELEASE,&em_key1,0,0) < 0) {			/* unlock EMERG record */
		db(EMERG_FILE_ID,CLOSE,&em_key1,0,0);				/* close EMERG file */
		return(-1);							/* if error, return */
	}

	if(db(EMERG_FILE_ID,CLOSE,&em_key1,0,0) < 0) 				/* close EMERG file */
		return(-1);							/* if error, return */

	if (em_ptr->drv_id <= 0)
		return(0);							/* driver not signed on yet */

	dr_ptr = (struct cisam_dr *)req_buf;
	bzero(dr_ptr, sizeof(struct cisam_dr));

	dr_ptr->fleet = fleet;
	dr_ptr->id = driver_id;

	if (db(DRIVER_FILE_ID,READ_KEYS,&dr_key1,ISEQUAL+ISLOCK,0) < 0) {	/* read DRIVER record */
		db(DRIVER_FILE_ID,RELEASE,&dr_key1,0,0);			/* unlock DRIVER record */
		return(-1);							/* if error, return */
	}

	dr_ptr->emerg_of_dt_tm = resolve_time;

	if (db(DRIVER_FILE_ID,UPDATE_REC,&dr_key1,0,0) < 0) {			/* update DRIVER record */
		db(DRIVER_FILE_ID,RELEASE,&dr_key1,0,0);			/* unlock DRIVER record */
		return(-1);							/* if error, return */
	}

	if(db(DRIVER_FILE_ID,RELEASE,&dr_key1,0,0) < 0) 			/* unlock DRIVER record */
		return(-1);							/* if error, return */
}
