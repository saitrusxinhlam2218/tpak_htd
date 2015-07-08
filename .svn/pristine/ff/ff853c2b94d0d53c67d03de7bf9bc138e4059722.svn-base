static char sz__FILE__[]="@(#)pkt_id.c	10.1.2.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  pkt_id.c; Rel: 10.1.2.1; Get date: 5/11/95 at 10:15:45
* @(#)  Last delta: 12/2/94 at 18:09:59
* @(#)  SCCS File: /taxi/sccs/s.pkt_id.c
*                                                                           *
* Copyright (c) 1991 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/

/****************************************************************************/
/* file pkt_id.c 			                                    */
/* routines for assigning pkt_ids and handling nak info                     */
/* taxi application 			                                    */
/* started 12 Jan 90 			                                    */
/* last update 16 Jan 1991                                                  */
/* M. R. Bradley 			                                    */
/*                                                                          */
/* DESIGN OVERVIEW:                                                         */
/* pkt_id in mmp_msg protocol are two bytes long (256 possible valuesr)     */
/* TaxiPak assigns pkt_id "00" to all messages except:                      */
/*	trip assignment                                                     */
/*	trip offer                                                          */
/*	messages to driver or taxi                                          */
/* This routine assigns specific pkt_id values                              */
/* These values are used to access information about the message used in    */
/*   case a nak is received. The information consists of:                   */
/*        call_nbr or msg_nbr as applicable                                 */
/*        vehicle_nbr                                                       */
/*        time msg sent                                                     */
/* A 256 element array is used to store this information.                   */
/* Array element 0 (pkt_id 00) is unused.                                   */
/* The async process sends the ack of messages with non 00 pkt_ids to       */
/*  dispatch. This information in turn is used to relase the space in       */
/*  in the above mentioned array for reuse.                                 */
/* In addition, to accomodate the case of lost messages, the pkt_id array   */
/*  is periodically checked (same frequency as kdt_stats output) for        */
/*  message that have been in too long (PLT_TIMEOUT).                       */
/* Space is released for old messages and an ERROR msg output.              */
/* To accomodate the case where more than 255 messages with non 00 pkt_ids  */
/*  are outstanding at any one time, provisions have been made to allocate  */
/*  space for another 255 pk_ids worth of information. Should more space    */
/*  be needed, more is allocated. This process can for all practical        */
/*  purposes go on indefinately. When allococated storage is nolonger       */
/*  needed it is freed. Notice of allocation and freeing is recorded via    */
/*  an error message                                                        */
/* The vehicle nbr is used to disambiguate the appropriate entry for a      */
/*  given pkt_id when there has been storage allocated (ie there are more   */
/*  than one entry for a given pkt_id). To provide further protection care  */
/*  is taken to insure if a pkt_id is assigned and if there is already an   */
/*  an entry with the same pkt_id and vehicle_nbr the earliet entry will be */
/*  accessed first when the ack or nack is received.                        */
/****************************************************************************/

#include <string.h>
#include <ctype.h>
#include <memory.h>
#include <sys/types.h>
#include <time.h>
#include "taxipak.h"
#include "Object.h"
#include "acc.h"
#include "STD.h"
#include "msg.h"
#include "msg_mmp.h"
#include "mad_error.h"
#include "mads_types.h"
#include "except.h"
#include "calls.h"
#include "veh_driv.h"
#include "timers.h"
#include "Timer_public.h"
#include "Packet_ID_private.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#ifdef DEBUG_LOTS
int   pdebug = 0;    /* to shut off reset of pkt_id entry */
#endif
#define MAX_PKT_ID 0xFE   /* number of unique pkt_ids */


#ifdef DEBUG_LOTS
char foo = 'a';
#endif

typedef struct pkt_id_bin_struct
{
#ifdef DEBUG_LOTS
   char  name;
#endif
   int   free_cnt;
   int   free_slot;
   struct pkt_id_bin_struct *next;
   struct pkt_id_bin_struct *previous;
   PKT_ID_ARRAY pkt_id_array[MAX_PKT_ID + 1];
}     PKT_ID_BIN, *PKT_ID_BIN_PTR;

PKT_ID_BIN first_pkt_id_bin;
PKT_ID_BIN_PTR first_pkt_id_bin_ptr = &first_pkt_id_bin;

extern char *scratch_ptr;
extern MSG_QUEUE_HDR outb;

#define PKT_TIMEOUT 600		/* hwo old before we discard */
#ifdef DEBUG_LOTS
int   min = MAX_PKT_ID;
#endif

#ifdef DEBUG_LOTS
static void
free_cnt_check(pkt_id_bin_ptr, start)
  PKT_ID_BIN_PTR pkt_id_bin_ptr;
  int start;
{
      int ii, cnt;
      cnt = start;
      for (ii = 1; ii <= MAX_PKT_ID; ii++)
      {
         if (pkt_id_bin_ptr->pkt_id_array[ii].type == NO_PKT_TYPE)
            cnt ++;
      }				/* end for */
      if (cnt != pkt_id_bin_ptr->free_cnt)
         printf("free_cnt_check eerror\n");
}
#endif

/************************************************************/
/* PKT_ID_BIN_PTR discard_old_pkt_entries()                 */
/* ether on demand or periodically free packet id slot      */
/* by comparing age (600 seconds)                           */
/* if we have a malloced bin see if we can FREE it          */
/************************************************************/

PKT_ID_BIN_PTR
discard_old_pkt_entries()
{
   PKT_ID_BIN_PTR pkt_id_bin_ptr;
   PKT_ID_BIN_PTR nxt_pkt_id_bin_ptr;    /* last one or previous one */
   PKT_ID_BIN_PTR rt_pkt_id_bin_ptr = NULL;
   int   ii;
   time_t now_time;

   now_time = time((time_t *) 0);
   pkt_id_bin_ptr = first_pkt_id_bin_ptr;
   while (pkt_id_bin_ptr != NULL)
   {
      nxt_pkt_id_bin_ptr = pkt_id_bin_ptr->next;

      for (ii = 1; ii <= MAX_PKT_ID; ii++)
      {
         if (pkt_id_bin_ptr->pkt_id_array[ii].type != NO_PKT_TYPE)
	    if ((now_time - pkt_id_bin_ptr->pkt_id_array[ii].pkt_time) > PKT_TIMEOUT)
	    {
	       sprintf(scratch_ptr, "old_pkt type %c, veh_nbr %d, nbr %d",
		    pkt_id_bin_ptr->pkt_id_array[ii].type,
		    pkt_id_bin_ptr->pkt_id_array[ii].veh_nbr,
		    pkt_id_bin_ptr->pkt_id_array[ii].nbr);
	       ERROR(' ', "", scratch_ptr);
	       pkt_id_bin_ptr->pkt_id_array[ii].type = NO_PKT_TYPE;
               pkt_id_bin_ptr->pkt_id_array[ii].veh_nbr = 0;
               pkt_id_bin_ptr->pkt_id_array[ii].nbr = 0;
	       pkt_id_bin_ptr->free_cnt++;

#ifdef ASSERT
               if (pkt_id_bin_ptr->free_cnt > MAX_PKT_ID)
               {
                  ERROR(' ',"","Error in free_cnt");
                  pkt_id_bin_ptr->free_cnt = MAX_PKT_ID;

               }
#endif
	       if (rt_pkt_id_bin_ptr == NULL)
	          rt_pkt_id_bin_ptr = pkt_id_bin_ptr;
	    }
      }				/* end for */

      /* can we free up this bin */
      if (pkt_id_bin_ptr->free_cnt >= MAX_PKT_ID)
      {
	 /* ok to free the storage if not the first bin */
	 if (pkt_id_bin_ptr != first_pkt_id_bin_ptr)
	 {
            pkt_id_bin_ptr->previous->next = pkt_id_bin_ptr->next;
            if (pkt_id_bin_ptr->next != NULL)
	       pkt_id_bin_ptr->next->previous = pkt_id_bin_ptr->previous;
            nxt_pkt_id_bin_ptr = pkt_id_bin_ptr->next;
	    ERROR(' ', "", " freeing pkt bin");
	    free(pkt_id_bin_ptr);
	    if (rt_pkt_id_bin_ptr == pkt_id_bin_ptr)
	       rt_pkt_id_bin_ptr = NULL;
	 }
      }

      pkt_id_bin_ptr = nxt_pkt_id_bin_ptr;
   }				/* end while */

   if (rt_pkt_id_bin_ptr == NULL)
   {
      nxt_pkt_id_bin_ptr = first_pkt_id_bin_ptr;
      while (nxt_pkt_id_bin_ptr->next != NULL)
          nxt_pkt_id_bin_ptr = nxt_pkt_id_bin_ptr->next;
      rt_pkt_id_bin_ptr = nxt_pkt_id_bin_ptr;
   }
   (void) Timer_create(
		       TIMER_TYPE, T_KDT_STATS,
		       TIMER_EXPIRE_TIME, 3600,
		       TIMER_NOTIFY_FUNC, discard_old_pkt_entries,
		       ATTRIBUTE_END);
   return (rt_pkt_id_bin_ptr);
}				/* end discard_old_pkt_entries */

/************************************************************/
/* static void init_pkt_bin(pkt_id_bin_ptr)                 */
/* initialize a bin (256 packet id slots)                    */
/************************************************************/

static void
init_pkt_bin(pkt_id_bin_ptr)
   PKT_ID_BIN_PTR pkt_id_bin_ptr;
{
   int   ii;
   for (ii = 1; ii <= MAX_PKT_ID; ii++)
   {
      pkt_id_bin_ptr->pkt_id_array[ii].type = NO_PKT_TYPE;
      pkt_id_bin_ptr->pkt_id_array[ii].nbr = 0;
      pkt_id_bin_ptr->pkt_id_array[ii].veh_nbr = 0;
   }
#ifdef DEBUG_LOTS
   pkt_id_bin_ptr->name = foo++;
#endif
   pkt_id_bin_ptr->next = NULL;
   pkt_id_bin_ptr->previous = NULL;
   pkt_id_bin_ptr->free_cnt = MAX_PKT_ID;
   pkt_id_bin_ptr->free_slot = 1;
}				/* end init_pkt_bin */

/************************************************************/
/* void init_pkt_ids()                                      */
/* initialize the first (static) bin                        */
/************************************************************/

void
init_pkt_ids()
{
   init_pkt_bin(first_pkt_id_bin_ptr);
}				/* end init_pkt_ids */

/************************************************************/
/* static PKT_ID_BIN_PTR get_free_pkt_bin()                 */
/* return a bid ptr with a free slot                        */
/* malloc a bin if no free slots available                  */
/************************************************************/

static PKT_ID_BIN_PTR
get_free_pkt_bin()
{
   PKT_ID_BIN_PTR pkt_id_bin_ptr;
   PKT_ID_BIN_PTR sv_pkt_id_bin_ptr;
   PKT_ID_BIN_PTR rt_pkt_id_bin_ptr;

   pkt_id_bin_ptr = first_pkt_id_bin_ptr;
   while (pkt_id_bin_ptr != NULL)
   {
      if (pkt_id_bin_ptr->free_cnt > 0)
	 return (pkt_id_bin_ptr);
      pkt_id_bin_ptr = pkt_id_bin_ptr->next;
   }
   /* have to get a new pkt_id_bin */

   /* see if we can reclaim any space */
   sv_pkt_id_bin_ptr = discard_old_pkt_entries();
   if (sv_pkt_id_bin_ptr->free_cnt)
      return (sv_pkt_id_bin_ptr);


   ERROR(' ',"","malloc of pkt_bin");
   if ((pkt_id_bin_ptr = (PKT_ID_BIN_PTR) malloc(sizeof(PKT_ID_BIN))) == NULL)
   {
      ERROR(' ', "", "malloc failure");
      return(first_pkt_id_bin_ptr);	/* better that nothing */
   } else
      init_pkt_bin(pkt_id_bin_ptr);
#ifdef DEBUG_LOTS
   printf("mallocing new pkt_id_bin %c\n",pkt_id_bin_ptr->name);
#endif
   sv_pkt_id_bin_ptr->next = pkt_id_bin_ptr;
   pkt_id_bin_ptr->previous = sv_pkt_id_bin_ptr;
   return (pkt_id_bin_ptr);
}				/* end get_free_pkt_bin */

/************************************************************/
/* static int get_free_pkt_slot(pkt_id_bin_ptr)             */
/* get a free slot from the argument bin                    */
/* if there isnt one (an ERROR) pick one                    */
/************************************************************/

static int
get_free_pkt_slot(pkt_id_bin_ptr)
   PKT_ID_BIN_PTR pkt_id_bin_ptr;
{
   int   ii, jj;
   int   slot = 0;
   double drand48();

   jj = pkt_id_bin_ptr->free_slot;
   if (pkt_id_bin_ptr->pkt_id_array[jj].type == NO_PKT_TYPE)
   {
      slot = pkt_id_bin_ptr->free_slot;
   }
   /* got to be one some where do linear search */
   else
      for (ii = 1; ii <= MAX_PKT_ID; ii++)
      {
	 if (pkt_id_bin_ptr->pkt_id_array[jj++].type == NO_PKT_TYPE)
	 {
	    slot = jj - 1;
	    pkt_id_bin_ptr->free_slot = jj - 1;
	    break;
	 }
	 if (jj > MAX_PKT_ID)
	    jj = 1;
      }
   if (!slot)
   {
      ERROR(' ', "", "NO free slots");
      pkt_id_bin_ptr->free_cnt = 0;
      /* just pick one and hope */
      slot = drand48() * MAX_PKT_ID;
      if (slot == 0)
	 slot++;
   } else
   {
      pkt_id_bin_ptr->free_slot++;
      pkt_id_bin_ptr->free_cnt--;
#ifdef DEBUG_LOTS
               free_cnt_check(pkt_id_bin_ptr, -1);
#endif
      if (pkt_id_bin_ptr->free_slot > MAX_PKT_ID)
	 pkt_id_bin_ptr->free_slot = 1;
   }
#ifdef DEBUG_LOTS
   if (pkt_id_bin_ptr->free_cnt < min)
      min = pkt_id_bin_ptr->free_cnt;
   printf("%d%c%d ", pkt_id_bin_ptr->free_cnt, pkt_id_bin_ptr->name, min);
   fflush(stdout);
#endif
   return (slot);
}				/* end get_free_pkt_slot */

/************************************************************/
/* void reset_pkt_entry(pkt_id, veh_nbr)                    */
/* free the pkt_id slot matching the pkt_id and the veh_nbr */
/************************************************************/

void
reset_pkt_entry(pkt_id, veh_nbr)
   int   pkt_id;
   int   veh_nbr;
{
   int   ii;
   int   cnt = 0;

   PKT_ID_BIN_PTR pkt_id_bin_ptr;
   PKT_ID_BIN_PTR sv_pkt_id_bin_ptr;

#ifdef DEBUG_LOTS
   if (pdebug)  /* so we can check malloc stuff */
      return;
#endif
   sv_pkt_id_bin_ptr = NULL;
   pkt_id_bin_ptr = first_pkt_id_bin_ptr;
   while (pkt_id_bin_ptr != NULL)
   {
      if (pkt_id_bin_ptr->pkt_id_array[pkt_id].veh_nbr == veh_nbr)
      {
	 pkt_id_bin_ptr->pkt_id_array[pkt_id].type = NO_PKT_TYPE;
         pkt_id_bin_ptr->pkt_id_array[pkt_id].veh_nbr = 0;
         pkt_id_bin_ptr->pkt_id_array[pkt_id].nbr = 0;
	 pkt_id_bin_ptr->free_cnt++;
#ifdef DEBUG_LOTS
               free_cnt_check(pkt_id_bin_ptr, 0);
#endif
	 if (pkt_id_bin_ptr->free_cnt >= MAX_PKT_ID)
	 {
	    /* ok to free the storage if not the first bin */
	    if (pkt_id_bin_ptr != first_pkt_id_bin_ptr)
	    {
#ifdef DEBUG_LOTS
	       printf("freeing  bin %c\n", pkt_id_bin_ptr->name);
#endif
               if (sv_pkt_id_bin_ptr != NULL)
	          sv_pkt_id_bin_ptr->next = pkt_id_bin_ptr->next;
               if (pkt_id_bin_ptr->next != NULL)
	          pkt_id_bin_ptr->next->previous = sv_pkt_id_bin_ptr;
	       ERROR(' ', "", " freeing pkt bin");
	       free(pkt_id_bin_ptr);
	    }
	    return;
	 }
	 /* pkt_id_bin_ptr->free_slot = pkt_id; */
	 return;
      }
      sv_pkt_id_bin_ptr = pkt_id_bin_ptr;
      pkt_id_bin_ptr = pkt_id_bin_ptr->next;
   }
   /* better not get here */
   sprintf(scratch_ptr, "Unable to free pkt entry id %d veh # %d", pkt_id, veh_nbr);
   ERROR(' ', "", scratch_ptr);
}				/* end reset_pkt_entry */

/************************************************************/
/* static void check_slot_sequence(pkt_id_bin_ptr, pkt_id)  */
/* chek for the exceptional case of same id/veh in          */
/* different bins - insure oldest is first                  */
/************************************************************/

static void
check_slot_sequence(pkt_id_bin_ptr, pkt_id)
   PKT_ID_BIN_PTR pkt_id_bin_ptr;
   int   pkt_id; 
{
   PKT_ID_ARRAY temp_slot;
   PKT_ID_BIN_PTR check_bin_ptr;
  
   check_bin_ptr = pkt_id_bin_ptr->next;
   while (check_bin_ptr != NULL)
   {
      if (pkt_id_bin_ptr->pkt_id_array[pkt_id].veh_nbr ==
          check_bin_ptr->pkt_id_array[pkt_id].veh_nbr)
      {
         if (pkt_id_bin_ptr->pkt_id_array[pkt_id].pkt_time >
             check_bin_ptr->pkt_id_array[pkt_id].pkt_time)
             {
#ifdef DEBUG_LOTS
                printf("check_slot_swap\n");
#endif
                temp_slot.type =     check_bin_ptr->pkt_id_array[pkt_id].type;
                temp_slot.nbr =      check_bin_ptr->pkt_id_array[pkt_id].nbr;
                temp_slot.pkt_time = check_bin_ptr->pkt_id_array[pkt_id].pkt_time;

                check_bin_ptr->pkt_id_array[pkt_id].type =     pkt_id_bin_ptr->pkt_id_array[pkt_id].type;
                check_bin_ptr->pkt_id_array[pkt_id].nbr =      pkt_id_bin_ptr->pkt_id_array[pkt_id].nbr;
                check_bin_ptr->pkt_id_array[pkt_id].pkt_time = pkt_id_bin_ptr->pkt_id_array[pkt_id].pkt_time;

                pkt_id_bin_ptr->pkt_id_array[pkt_id].type =     temp_slot.type;
                pkt_id_bin_ptr->pkt_id_array[pkt_id].nbr =      temp_slot.nbr;
                pkt_id_bin_ptr->pkt_id_array[pkt_id].pkt_time = temp_slot.pkt_time;
                break;
             }
      }
      check_bin_ptr = check_bin_ptr->next;
   }         /* end while */
}               /* end check_slot_sequence */

/************************************************************/
/* int send_pkt_id(type, nbr, veh_nbr)                      */
/* get a pkt_id by get a free pkt_id slot                   */
/* set slot values to the argement values                   */
/* set the pkt_id field of the outbound message             */
/************************************************************/

int
send_pkt_id(type, nbr, veh_nbr)
   char  type;
   int  nbr;
   int   veh_nbr;
{
   char  pkt_id_str[3];
   PKT_ID_BIN_PTR pkt_id_bin_ptr;
   int   pkt_id;

   pkt_id_bin_ptr = get_free_pkt_bin();
   pkt_id = get_free_pkt_slot(pkt_id_bin_ptr);
#ifdef DEBUG_LOTS
   if (pkt_id > MAX_PKT_ID)
      printf("bad_pkt_id %d\n");
#endif
   sprintf(pkt_id_str, "%02X", pkt_id);
   memcpy(outb.pkt_id, pkt_id_str, sizeof(outb.pkt_id));

   pkt_id_bin_ptr->pkt_id_array[pkt_id].type = type;
   pkt_id_bin_ptr->pkt_id_array[pkt_id].nbr = nbr;
   pkt_id_bin_ptr->pkt_id_array[pkt_id].veh_nbr = veh_nbr;
   pkt_id_bin_ptr->pkt_id_array[pkt_id].pkt_time = time((time_t *) 0);
   check_slot_sequence(pkt_id_bin_ptr, pkt_id);
   return (pkt_id);
}				/* end send_pkt_id */

/************************************************************/
/* void clr_pkt_id()                                        */
/* clear the pkt_id field of the outbound message           */
/************************************************************/

void
clr_pkt_id()
{
   memcpy(outb.pkt_id, "00", sizeof(outb.pkt_id));
}

/************************************************************/
/* PKT_ID_ARRAY_PTR find_pkt_id_array(pkt_id, veh_nbr, type)*/
/* find the first slot matchin the id and veh_nbr           */
/************************************************************/

PKT_ID_ARRAY_PTR
find_pkt_id_array(int pkt_id, int veh_nbr, char type)
{
   PKT_ID_BIN_PTR pkt_id_bin_ptr;

   pkt_id_bin_ptr = first_pkt_id_bin_ptr;
   while (pkt_id_bin_ptr != NULL)
   {
      if (pkt_id_bin_ptr->pkt_id_array[pkt_id].veh_nbr == veh_nbr)
	 if (type != NO_PKT_TYPE)
	 {
	    if (pkt_id_bin_ptr->pkt_id_array[pkt_id].type == type)
	       return (&pkt_id_bin_ptr->pkt_id_array[pkt_id]);
	 } else
	    return (&pkt_id_bin_ptr->pkt_id_array[pkt_id]);
      pkt_id_bin_ptr = pkt_id_bin_ptr->next;
   }
   return ((PKT_ID_ARRAY_PTR) NULL);
}				/* end find_pkt_id_array */

/************************************************************/
/* char get_pkt_type(pkt_id, veh_nbr)                       */
/* return the type field of the pkt_id                      */
/************************************************************/

char
get_pkt_type(pkt_id, veh_nbr)
   int   pkt_id;
   int   veh_nbr;
/* return the first match */
{
   PKT_ID_ARRAY_PTR pkt_id_array_ptr;

   pkt_id_array_ptr = find_pkt_id_array(pkt_id, veh_nbr, NO_PKT_TYPE);
   if (pkt_id_array_ptr != NULL)
   {
#ifdef DEBUG_LOTS
      if (pkt_id_array_ptr->type == ASSIGN_PKT_TYPE)
	 printf("C\n");
#endif
      return (pkt_id_array_ptr->type);
   } else
      return (NO_PKT_TYPE);
}				/* end get_pkt_type */

/************************************************************/
/* int  get_pkt_num(pkt_id, veh_nbr)                        */
/* return the nbr field of the pkt_id                       */
/************************************************************/

int
get_pkt_num(pkt_id, veh_nbr)
   int   pkt_id;
   int   veh_nbr;
/* return the first match */
{
   PKT_ID_ARRAY_PTR pkt_id_array_ptr;

   pkt_id_array_ptr = find_pkt_id_array(pkt_id, veh_nbr, NO_PKT_TYPE);
   if (pkt_id_array_ptr != NULL)
      return (pkt_id_array_ptr->nbr);
   else
      return (0);
}				/* end get_pkt_nbr */


PKT_ID_ARRAY_PTR
find_pkt_id_by_veh(int veh_nbr, char type)
{
   PKT_ID_BIN_PTR pkt_id_bin_ptr, nxt_pkt_id_bin_ptr;
   int ii;
   
   pkt_id_bin_ptr = first_pkt_id_bin_ptr;
   while (pkt_id_bin_ptr != NULL)
   {
     nxt_pkt_id_bin_ptr = pkt_id_bin_ptr->next;

     for (ii=1; ii <= MAX_PKT_ID; ii++)
       {
	 if ( (pkt_id_bin_ptr->pkt_id_array[ii].veh_nbr == veh_nbr) &&
	     (pkt_id_bin_ptr->pkt_id_array[ii].type == type ) )
	   {
	       return (&pkt_id_bin_ptr->pkt_id_array[ii]);
	   }
       }
     pkt_id_bin_ptr = nxt_pkt_id_bin_ptr;
   }
   return ((PKT_ID_ARRAY_PTR) NULL);
}				/* end find_pkt_id_array */
