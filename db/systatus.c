static char sz__FILE__[]="@(#)systatus.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  systatus.c; Rel: 10.1.2.1; Get date: 4/20/95 at 09:48:06
 * @(#)  Last delta: 12/2/94 at 18:25:40
 * @(#)  SCCS File: /taxi/sccs/s.systatus.c
 *                                                                           *
 *****************************************************************************/

#include <time.h>
#include "mad_error.h"
#include "mad_ipc.h"
#include "systat.h"

struct stat *redncy_status = NULL;  /* This MUST be init'ed by the main program */

/**************************************************************
/*  set_status(state)
/*
/*     Set the status bits in the system shared memory to the
/*     desired state.
/*
/*         3/18/88          Jack Chou
/*
/*     ROUTINES used:
/*
/*
/*     RETURN:
/*         0         OK	
/*         -1        failed
/**************************************************************/

set_status(state)
char state;
{
#ifdef DEBUG
sprintf(trace_str, "ENTERING set_status:   state= %x\n", state);
TRACE(trace_str);
#endif

if (redncy_status == 0)      /* if shared is not attached, then forget it */
      return(-1);

#ifdef DEBUG
sprintf(trace_str, "   cur status= %d %d %d %d (b set)\n",
   redncy_status->dispatch, redncy_status->ncp,
   redncy_status->primary, redncy_status->redncy);
TRACE(trace_str);
#endif

switch(state)
   {
   case RDCY_STAT:     /* redundancy is running */
      redncy_status->redncy = 1;
      break;

   case PRIM_STAT:     /* primary host */
      redncy_status->primary = 1;
      break;
 
   case NRDCY_STAT:    /* non-redundancy state */
      redncy_status->redncy = 0;
      break;

   case SECD_STAT:     /* non-primary or secondary host */
      redncy_status->primary = 0;
      break;

   case DISP_STAT:     /* dispatch is running */
      redncy_status->dispatch = 1;
      break;

   case NCP_STAT:      /* NCP is running */
      redncy_status->ncp = 1;
      break;

   case NDISP_STAT:    /* dispatch is not running */
      redncy_status->dispatch = 0;
      break;

   case NNCP_STAT:     /* NCP is not running */
      redncy_status->ncp = 0;
      break;

   default:
      sprintf(trace_str, "set_status Unknown status type %x", state);
      ERROR(' ', "", trace_str);
      return(-1);
   }

#ifdef DEBUG
sprintf(trace_str, "   cur status= %d %d %d %d (a set)\n",
   redncy_status->dispatch, redncy_status->ncp,
   redncy_status->primary, redncy_status->redncy);
TRACE(trace_str);
sprintf(trace_str, "LEAVING set_status\n");
TRACE(trace_str);
#endif

return(0);
}

/**************************************************************
/*  is_status(state)
/*
/*     Check if system is in a specified state by checking the
/*     status bits in the system shared memory.
/*
/*         3/18/88          Jack Chou
/*
/*     ROUTINES used:
/*
/*
/*     RETURN:
/*          -1       failed (likely get_offsets error)
/*          0        not true
/*          1        true
/**************************************************************/

is_status(state)
char state;
{
int ret_val;

redncy_status = 0;
if (redncy_status == 0)     /* share memory is not attached, forget it */
   return(-1);

#ifdef DEBUG
sprintf(trace_str, "is_status:   state= %x\n", state);
TRACE(trace_str);
#endif

#ifdef REDUNDANT_YES
switch(state)
   {
   case RDCY_STAT:     /* redundancy is running */
      if (redncy_status->redncy)
         ret_val = 1;
      else
         ret_val = 0;
      break;

   case PRIM_STAT:     /* primary host */
      if (redncy_status->primary)
         ret_val = 1;
      else
         ret_val = 0;
      break;

   case DISP_STAT:     /* dispatch is running */
      if (redncy_status->dispatch)
         ret_val = 1;
      else
         ret_val = 0;
      break;

   case NCP_STAT:      /* NCP is running */
      if (redncy_status->ncp)
         ret_val = 1;
      else
         ret_val = 0;
      break;

   case NRDCY_STAT:    /* non-redundancy state */
      if (redncy_status->redncy)
         ret_val = 0;
      else
         ret_val = 1;
      break;

   case SECD_STAT:     /* non-primary or secondary host */
      if (redncy_status->primary)
         ret_val = 0;
      else
         ret_val = 1;
      break;
 
   case NDISP_STAT:    /* dispatch is not running */
      if (redncy_status->dispatch)
         ret_val = 0;
      else
         ret_val = 1;
      break;

   case NNCP_STAT:     /* NCP is not running */
      if (redncy_status->ncp)
         ret_val = 0;
      else
         ret_val = 1;
      break;

   default:
      sprintf(trace_str, "set_status: Unknown status type %x", state);
      ERROR(' ', "", trace_str);
      return(-1);
   }
#endif

return(ret_val);
}

/**************************************************************
/*  is_prim_redncy()
/*
/*     Check if system is a primary host and in rednudant state.
/*
/*         3/18/88          Jack Chou
/*
/*     ROUTINES used:
/*         is_status
/*
/*     RETURN:
/*          0        not true or can't determine
/*          1        true
/**************************************************************/

is_prim_redncy()
{
if (redncy_status == 0)   /* shared memory is NOT attached, forget it */
   return(0);

if (is_status(PRIM_STAT) && is_status(RDCY_STAT))
   return(1);
else
   return (0);
}
