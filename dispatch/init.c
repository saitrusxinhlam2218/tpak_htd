static char sz__FILE__[]="@(#)init.c	10.1.2.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  init.c; Rel: 6.35.0.0; Get date: 4/8/92 at 16:24:38
* @(#)  Last delta: 1/16/92 at 16:34:10
* @(#)  SCCS File: /taxi/sccs/s.init.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/

/* Based on R3 version 5.17 */

#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <isam.h>
#include <string.h>
#include <malloc.h> 
#include <memory.h>
#include <time.h>

#include "taxipak.h"
#include "Object.h"
#include "List_private.h"
#include "List_public.h"
#include "Zone_public.h"
#include "Zone_private.h"
#include "Fleet_public.h"
#include "Fleet_private.h"
#include "Fleet_db.h"
#include "Zone_db.h"
#include "Zonepoly_db.h"
#include "Fleet_lists_public.h"
#include "Driver_public.h"
#include "Timer_public.h"
#include "Call_public.h"
#include "Call_private.h"
#include "TimeCalls_private.h"
#include "Call_db.h"
#include "Vehicle_public.h"
#include "Vehicle_private.h"
#include "Vehicle_db.h"
#include "Call_lists_public.h"
#include "Street_db.h"
#include "Attribute_db.h"
#include "System_control_db.h"
#include "Res_periods_db.h"
#include "Reservation_Period.h"
#include "Holiday_db.h"
#include "Time_call_db.h"
#include "Meter_action_db.h"
#include "Call_msg_db.h"
#include "Menu_db.h"
#include "Excpt_type_db.h"
#include "Excpt_group_db.h"
#include "Exception_private.h"
#include "TimeCalls_public.h"
#include "TimeCalls_private.h"
#include "Channel_db.h"
#include "Tlg_db.h"
#include "taxi_db.h"
#include "sprintf.h"
#include "path.h"
#include "mad_error.h"
#include "mem_mgmt.h"
#include "except.h"
#include "mad_ipc.h"
#include "ui_ipc.h"
#include "dispatch.h"
#include "user.h"
#include "timers.h"
#include "menu.h"
#include "language.h"
#include "async.h"
#include "kdt.h"         /* temp */
#include "enhance.h"
#include "switch_ext.h"
#include "GPS_private.h"
#include "IMSI_db.h"
#include "S_Shift_db.h"
#include "S_Sched_db.h"

typedef struct sortavlq
{
        short fleet;
        int  nbr;
        char mobile_rf_id[MID_SIZE + 1];
        short status;
        short last_zone_posted;
        long last_time_posted;
	int last_driver;
        struct sortavlq * next;
} SORTAVLQ;


#define BUILD_CMNT "building"
#define EXIST_CMNT "already exists, left unchanged"
extern struct veh_driv *get_veh_id();
extern struct offsets *offset;				/* offsets of structures in shared memory */
extern struct fleets *fleet[FLEET_MAX];			/* shared memory fleet structure */
extern struct excepts excpt_rec_buf;			/* exception buffer */
extern struct excpt_index except_idx[EXCEPT_MAX]; 	/* sorted array of exception index structures */
struct veh_driv *veh_temp[VEH_DRIV_MAX];
extern char *shmptr;					/* pointer to allocated shared memory */
extern int menus_used;					/* number of menus used in system at current time */
extern struct e_intervals e_interval[EX_PRI_MAX];  	/* interval record blocks for exceptions */
extern int  veh_used; 					/* number of vehicles read during initialization */
extern char req_buf[BUFSIZ];
extern struct drvr_info driver_info;
extern char *scratch_ptr;
extern int warm_start;
int RES_add_period( RES_PERIODS *, int, int, short, float );
int zone_rebuild( ZONE_HNDL );
FILE *dummy_file;

struct exception_count_struct *exception_count;
CALL * last_free_call;             /* for debugging */
int free_calls_nbr = 0;         /* for debugging */
int num_tc = 0;

int veh_stats_inc = 100;                                /* how many veh to write drv stats for at one time */
int up_veh_idx( struct veh_driv * );
extern time_t mads_time;
RET_STATUS veh_populate_lists();
extern FLEET_VEH_ID_LIST_HNDL fleet_veh_id_list;
extern FLEET_PKT_ID_LIST_HNDL fleet_pkt_id_list;
extern int test_data;

/****************************************************************************
* make_mem -	allocate the shared memory segment for all shared structures 
* 
* The memory is laid out in a predetermined order and fixed at a given
* address.  Therefore, it is possible, given one shared memory pointer, 
* to access all the in memory structures.  Non-owners who attatch the shared 
* segment will simply set the offset structure to point the address returned 
* after the shmat() call. Then all structures within shared memory can be
* accessed by pointers within the offset structure.
*****************************************************************************/
char *make_mem()
{
	extern int  shmid;	/* identifier of shared memory */
	int size;
	struct offsets *offset_shmptr;

											/* get the size of all structures */
        printf("%d users\n", USER_MAX);
	size = sizeof(struct offsets) +	FLEET_MAX * sizeof(struct fleets) 
   		+ CALLS_MAX * sizeof(struct calls) + USER_MAX * sizeof(struct users) 
		+ EXCEPT_MAX * sizeof(struct excepts) 
		+ MENU_MAX * sizeof(struct menus)
		+ EXCEPT_DESC_MAX * sizeof(struct excpt_desc)
	        + sizeof(struct exception_count_struct)
		+ RES_PERIOD_MEM_SIZE
		+ HOLIDAY_MEM_SIZE;
#ifdef NAMERICA
	size += TLG_MEM_SIZE + CHANNEL_MEM_SIZE;
#endif

	if((shmid =shmget(SHMKEY,size,0777 | IPC_CREAT  )) == -1){			/* get a shared memory id */
		ERROR(' ',"","make_mem - couldn't get shared memory id.");
		fprintf(stderr,"dispatch is already running or shared memory still active\n");
		cleanup();
	}

	fprintf(stdout,"\n%d bytes allocated for shared memory\n",size);

	shmptr=shmat(shmid, (const void *)0x0a00000010000000, 0);
	//	shmptr=shmat(shmid,0x1000000, SHM_RND);			/* put shared memory at fixed address */

	memset(shmptr,'\0',size);
	offset = offset_shmptr = (struct offsets*)shmptr;			/* place offset struct at start of shared memory */
	return((char *)++offset_shmptr); 						/* return address of free memory */
}


/****************************************************************************
build_zones - 	Read the fleet file for fleets being used.  Allocate memory
for the the zone structures within the fleets and then populate the zones with
data read from disk. Backup zone information is populated and inverted
backup zone information is initialized.
*****************************************************************************/
char *build_zones(fleet_shmptr)
struct fleets *fleet_shmptr;
{
        FLEET_REC  *fl_ptr;
	ZONE_REC   *czone_tmp_ptr;		/* pointer to cisam  returned zone structure */
	ZONE_POLY_REC zonepoly_rec;
	struct zones *zone_tmp_ptr;		/* pointer to zone structure */
	register int h,i,j,k;			/* counter variables */
	int mode, mode2;
	int sup_zn_idx;
	struct zones *zone_shmptr;
	int first_zone_fleet;
	int point_index;
	int done = 0;

	struct cisam_sh s_sched;
	struct cisam_ss s_shift;

	offset->fleet = fleet_shmptr;
	if (db(FLEET_FILE_ID, OPEN, &fl_key1, ISINOUT | ISMANULOCK, 0) < 0)
	  {				/* open the file */
	    ERROR(' ', FLEET_FILE, "build_zones - cannot open file\n");
	    cleanup();
	  }

	fl_ptr = (FLEET_REC *)req_buf;
	
	if(db(ZONE_FILE_ID,OPEN,&zn_key1,ISINOUT | ISMANULOCK,0) < 0){		/*  open the file */
	  ERROR(' ',ZONE_FILE,"build_zones - cannot open file\n");
	  cleanup();
	}

	db_open(ZONEPOLY_FILE_ID, ISINOUT + ISMANULOCK);
	
	czone_tmp_ptr = (struct cisam_zn*)req_buf;				/* point to the req_buffer */

	for (h=0;h<FLEET_MAX;h++)
	  fleet[h] = fleet_shmptr++;

	fleet_veh_id_list = Fleet_veh_id_list_create();
	fleet_pkt_id_list = Fleet_pkt_id_list_create();

	for(h=0;h<FLEET_MAX;h++){ 						/* loop through for each fleet that exists */
		sup_zn_idx = 0;

		fl_ptr->nbr = h;
		if (db(FLEET_FILE_ID, READ_KEYS, &fl_key2, ISEQUAL, 0) < 0)
		  {				/* if fleet record is not returned */
		    if (iserrno == ENOREC)
		      {
			fleet[h]->active = SPACE;	/* mark fleet as unused */
			continue;		/* don't do anything else for this fleet */
		      }
		  }
		first_zone_fleet = 0;
		(void) Fleet_create(h);
		fleet[h]->criteria.ncp_on = 1;
		Fleet_rec_2_obj(fleet[h], fl_ptr);
		
		if ((zone_tmp_ptr = (struct zones *) malloc(sizeof(struct zones))) == NULL) 
		  {				/* allocate memory for a zone */
		    ERROR(' ', "", "build_zones - could not allocate zone memory.");
		    cleanup();
		  }
		memset((char *) zone_tmp_ptr, '\0', sizeof(struct zones));		

		fleet[h]->zone_ptr[0] = zone_tmp_ptr;
		
		czone_tmp_ptr->fleet = fleet[h]->fleet_id;
		czone_tmp_ptr->nbr = 1;
		mode = ISGTEQ;

		while( db(ZONE_FILE_ID,READ_KEYS,&zn_key1,mode,0) >= 0){	/* if zone record is returned from isam call*/
			mode = ISNEXT;
			if( czone_tmp_ptr->fleet != fleet[h]->fleet_id)		/* invalid fleet */
				break;
						
			if( czone_tmp_ptr->type == '3'){			/* save the superzones for leadtime and estimates */
				if( sup_zn_idx < MAX_SUP_ZONES ){
					fleet[h]->super_zone[sup_zn_idx].nbr = czone_tmp_ptr->nbr;
					++sup_zn_idx;
				}
				continue;
			}
			
			fleet[h]->zone_ptr[czone_tmp_ptr->nbr] = (struct zones *)Zone_create(czone_tmp_ptr->nbr, h, czone_tmp_ptr);
			zonepoly_rec.fleet = fleet[h]->fleet_id;
			zonepoly_rec.nbr = czone_tmp_ptr->nbr;
			zonepoly_rec.seq_nbr = 1;
			mode2 = ISEQUAL;
			point_index = 0;
			done = 0;
			while ( !done ) 
			  {
			    if ( db_read_key(ZONEPOLY_FILE_ID, &zonepoly_rec, &znpoly_key1, mode2) == SUCCESS )
			      {
				mode2 = ISNEXT;
				if ( zonepoly_rec.nbr != czone_tmp_ptr->nbr )
				  done = TRUE;
				else
				  {
				    fleet[h]->zone_ptr[czone_tmp_ptr->nbr]->zone_poly[point_index].x = zonepoly_rec.pointx;
				    fleet[h]->zone_ptr[czone_tmp_ptr->nbr]->zone_poly[point_index].y = zonepoly_rec.pointy;
				    ++point_index;
				  }
                                if ( point_index > MAX_POLY_POINTS )
                                  break;
			      }
			    else
			      done = 1;
			  }
			if ( point_index > 0 )
			  {
			    fleet[h]->zone_ptr[czone_tmp_ptr->nbr]->poly_points = point_index;
			    ZonePolyCalculateCentroid( (ZONE_HNDL)fleet[h]->zone_ptr[czone_tmp_ptr->nbr] );
			  }
			
			if ( first_zone_fleet == 0 )
			  {
			    (void) Timer_create(
						TIMER_TYPE, T_ZONE_REBUILD,
						TIMER_EXPIRE_TIME, 20,
						TIMER_NOTIFY_FUNC, zone_rebuild,
						TIMER_DATA, (ZONE_HNDL)fleet[h]->zone_ptr[czone_tmp_ptr->nbr],
						ATTRIBUTE_END );
			    first_zone_fleet = 1;
			  }

		}
		fleet[h]->zone_ptr[WAKEUP_ZONE] = (struct zones *)Zone_create(WAKEUP_ZONE, h, NULL );
	}
	db(FLEET_FILE_ID, CLOSE, &fl_key1, 0, 0);	/* close the file */
	db(ZONE_FILE_ID,CLOSE,&zn_key1,0,0);					/*  close the file */
	db_close(ZONEPOLY_FILE_ID);

	if ( db(S_SHIFT_FILE_ID, OPEN, &ss_key1, ISINOUT | ISMANULOCK, 0) < 0 ){
	  ERROR(' ',S_SHIFT_FILE, "...unable to open shift table\n");
	}

	if ( db(S_SCHED_FILE_ID, OPEN, &s_sh_key1, ISINOUT | ISMANULOCK, 0) < 0 ){
	  ERROR(' ',S_SCHED_FILE, "...unable to open shift table\n");
	}

	if ( db(S_VERSION_FILE_ID, OPEN, &sv_key1, ISINOUT | ISMANULOCK, 0) < 0 ){
	  ERROR(' ',S_VERSION_FILE, "...unable to open shift version table\n");
	}

	return((char*)fleet_shmptr); 						/* leave pointing to free shared space */
}

/****************************************************************************
build_inv_zones	- Populate the incore inverted zone stucture from the 
information contained in the in memory zone structure.
*****************************************************************************/
build_inv_zones(fl_num)
int fl_num;
{
  register int i,j,k,l,p;		/* loop counters */
  int tmp_zone;
  int avail=0;
  int found_inv_zone;
  struct zones *zone_tmp_ptr, *zone2_tmp_ptr;
  
#ifdef DEBUG
  if(offset->prattle >= LEVEL1){
    sprintf(trace_str,"build_inv_zones - fleet number = %d \n",fl_num);
    TRACE(trace_str);
  }
#endif

  if (fleet[fl_num]->active == YES)
   {				/* if the fleet is used */
      for (i = 1; i < ZONE_MAX; i++)
      {				/* loop through all its zones */
	 zone_tmp_ptr = fleet[fl_num]->zone_ptr[i];	/* get pointer to zone
							 * structure */
	 if (zone_tmp_ptr != 0)
	 {			/* if zone is being used */
	    for (j = 0; j < LEVEL_MAX; j++)
	    {			/* loop through levels of that zone */
	       for (k = 0; k < BCK_UP_MAX; k++)
	       {		/* loop through backups for given level */
		  /* get a backup zone for this zone */
		  tmp_zone = zone_tmp_ptr->level_number[j].bck_up_zone[k];
		  if (tmp_zone > 0 && tmp_zone < ZONE_MAX)
		  {		/* if the back up  zone exists */
		     /* get a pointer to the backup zone */
                    if ( tmp_zone == 888 )
                      continue;
                    
		     zone2_tmp_ptr = fleet[fl_num]->zone_ptr[tmp_zone];
		     if ( (zone2_tmp_ptr == NULL) )
			/* this is an improper backup zone */
			/* there is probably a problem in the data base */
			/* try to correct the error now */
		     {
			sprintf(trace_str, "zone %d illegal backup zone %d - fleet %d", 
                                            zone_tmp_ptr->zone_nbr,tmp_zone, fl_num);
			ERROR(' ', "", trace_str);
			zone_tmp_ptr->level_number[j].bck_up_zone[k] = 0;
			continue;
		     }
		     /* if the backup list is not full */
		     if (zone2_tmp_ptr->level_number[j].num_in_inv < INV_BCK_UP_MAX)
		     {
			/* increment the number of inverted backups */
		        /* if the zone is not already present       */
		        found_inv_zone = 0;
		        for ( p = 0; p < INV_BCK_UP_MAX; p++ )
			  {
			    if (zone2_tmp_ptr->level_number[j].inv_bck_up_zone[p] == i)
			      {
				found_inv_zone = 1;
				break;
			      }
			    if (zone2_tmp_ptr->level_number[j].inv_bck_up_zone[p] == 0)
			      break;
			  }

			if ( !found_inv_zone )
			  {
			    avail = zone2_tmp_ptr->level_number[j].num_in_inv++;
			    /*
			     * put this zone in the inv backup zones for this
			     * zones backup
			     */
			    zone2_tmp_ptr->level_number[j].inv_bck_up_zone[avail] = i;
			  }
		     }
		  }
	       }
	    }
	 }
      }
   }
}

#ifdef DEBUG
/****************************************************************************
print_zones -	Print the in memory zone information for fleets which exist
*****************************************************************************/
print_zones()
{
	register int h,i,j,k;
	struct zones *zone_tmp_ptr;

#ifdef DEBUG
	sprintf(trace_str,"print_zones \n");
	TRACE(trace_str);
#endif

	for(h=0;h<FLEET_MAX;h++){	
		if(fleet[h]->active == YES){
			for(i=1;i<ZONE_MAX;i++){	
				zone_tmp_ptr=fleet[h]->zone_ptr[i];
				if(zone_tmp_ptr != 0){
					printf("\n %d %x",i,zone_tmp_ptr);
					for(j=0;j<LEVEL_MAX;j++){
						printf("\n");
						for(k=0;k<BCK_UP_MAX;k++){
							printf(" %3d ",zone_tmp_ptr->level_number[j].bck_up_zone[k]);
						}
					}
				}
			}
		}
	}
}

/****************************************************************************
print_inv_zones - Print the in memory inverted zone information for fleets 
which exist
*****************************************************************************/
print_inv_zones()
{
	register int h,i,j,k;
	struct zones *zone_tmp_ptr;

#ifdef DEBUG
	sprintf(trace_str,"print_inv_zones \n");
	TRACE(trace_str);
#endif

	for(h=0;h<FLEET_MAX;h++){	
		if(fleet[h]->active == YES){
			for(i=1;i<ZONE_MAX;i++){	
				zone_tmp_ptr=fleet[h]->zone_ptr[i];
				if(zone_tmp_ptr != 0){
					printf("\n %d %x",i,zone_tmp_ptr);
					for(j=0;j<LEVEL_MAX;j++){
							printf("\n");
						for(k=0;k<BCK_UP_MAX;k++){
							printf(" %3d ",zone_tmp_ptr->level_number[j].inv_bck_up_zone[k]);
						}
					}
				}
			}
		}
	}
}
#endif

/****************************************************************************
build_calls - Initialize a pool of call nodes to point to each succesive 
array element to form a free node pool. The structure will start at the
address passed to it upon invocation. The next available call in zone
pointers will be initailized to NULL at this time.
*****************************************************************************/
char *build_calls(call_shmptr)
struct calls *call_shmptr;
{
	register int i;
	struct calls *tmp_call_ptr;

	offset->call = tmp_call_ptr = call_shmptr;				/* set up call block pointer */

	for(i=0;i<CALLS_MAX-1;i++){						/* loop for maximum number of calls */
		call_shmptr->next = ++tmp_call_ptr;				/* set up each call to point to the next call */
		++call_shmptr;
		free_calls_nbr++;
	}
	last_free_call = call_shmptr;
	call_shmptr->next = NULL;						/* make the last call point to a null call */
	call_shmptr++;								/* add space for the last node */

	offset->free_call_idx = 1;						/* first index available for new call */
	offset->free_call = offset->call;					/* first call node available for a new call */

	return((char*)call_shmptr); 						/* leave pointing to free shared space */
}

/****************************************************************************
build_users - allocate space for user profiles and information
*****************************************************************************/
char *build_users(user_shmptr)
struct users *user_shmptr;
{
	register i;
	
#ifdef DEBUG
	if(offset->prattle >= LEVEL1){
		sprintf(trace_str,"build_users : user_shmptr = %x \n",user_shmptr);
		TRACE(trace_str);
	}
#endif

	offset->user = user_shmptr;						/* initialize pointer to user block */

	for(i=0;i<USER_MAX;i++){
		user_shmptr++;
	}
	
	return((char *)user_shmptr);						/* return pointer to the end of the user block */
}

/****************************************************************************
build_except - allocate space for exception table
*****************************************************************************/
char *build_except(except_shmptr)
struct excepts *except_shmptr;
{
	register int i;
	struct excepts *tmp_except_ptr;
	LIST *list_shmptr, *tmp_list_ptr;
	LIST_ELEMENT *element_shmptr, *tmp_element_ptr;

	offset->except = tmp_except_ptr = except_shmptr;			/* initialize pointer to exception block */

	for(i=0;i<EXCEPT_MAX-1;i++){	
		except_shmptr->next = ++tmp_except_ptr;
		++except_shmptr;
	}

 	except_shmptr->next = NULL;						/* make the last exception point to NULL */
	except_shmptr++;							/* add space for the last node */

	offset->free_except_idx = 0;						/* first index available for new exception */
	offset->free_except = offset->except;					/* first node available for a new exception */

	offset->first_except = NULL; 						/* initialize exceptions to be empty */

	for(i=0;i<EX_PRI_MAX;i++)
	  {
	    e_interval[i].first = NULL;
	    e_interval[i].next = NULL;
	    e_interval[i].prev = EOL;
	    e_interval[i].next = EOL;
	  }

	return((char *)except_shmptr);						/* return the end of the exceptions */
}

/****************************************************************************
build_except_desc - allocate space and populate  exception description
*****************************************************************************/
char *build_except_desc(except_desc_shmptr)
struct excpt_desc *except_desc_shmptr;
{
	register int i;
	struct cisam_et *et_ptr;
	int mode=ISGTEQ;

#ifdef DEBUG
	if(offset->prattle >= LEVEL1){
		sprintf(trace_str,"build_except_desc : except_desc_shmptr = %x \n",except_desc_shmptr);
		TRACE(trace_str);
	}
#endif

	et_ptr = (struct cisam_et*)req_buf;
	offset->except_desc =  except_desc_shmptr;			/* initialize pointer to exception block */

	if(db(EXC_TYPE_ID,OPEN,&et_key2,ISINOUT | ISMANULOCK,0) < 0){		/*  open the file */
		ERROR(' ',EXCTYPE_FILE,"build_except_desc - cannot open file\n");
		cleanup();
	}

	for(i=1;i<=EXCEPT_DESC_MAX;i++){
		et_ptr->nbr = i;
		if( db(EXC_TYPE_ID,READ_KEYS,&et_key1,mode,0) >= 0){		/* read in all the description records */
			except_desc_shmptr->group_nbr = et_ptr->group_nbr;
			strcpy(except_desc_shmptr->short_desc,et_ptr->short_desc);
		}
		++except_desc_shmptr;
	}

	db(EXC_TYPE_ID,CLOSE,&et_key2,0,0);

	return((char *)except_desc_shmptr);					/* return the end of the exceptions */
}

char *build_holidays( pHolidays )
     HOLIDAYS     *pHolidays;
{
  int               i, rc, mode;
  struct cisam_hl   *pHolRec;
  HOLIDAYS          *holidays[ MAX_HOLIDAYS ], *hol_shmptr;


  offset->Holidays = ( HOLIDAYS * ) pHolidays;
  hol_shmptr = pHolidays;
  bzero( pHolidays, HOLIDAY_MEM_SIZE );
  if ( pHolidays == NULL )
    return( NULL );
  for ( i = 0; i < MAX_HOLIDAYS; i++ )
    ++hol_shmptr;

  pHolRec = ( struct cisam_hl * ) req_buf;
  holidays[0] = ( HOLIDAYS * ) pHolidays;

  if (db(HOLIDAY_FILE_ID, OPEN, &hl_key1, ISINOUT | ISMANULOCK, 0) < 0)
    {		
      ERROR(' ', HOLIDAY_FILE, "Error OPENing holiday file");
      exit(-1);
    }
  i = 0;

  pHolRec->mon = 0;
  pHolRec->day = 0;
  pHolRec->year = 0;
  for ( mode = ISFIRST; (rc = db( HOLIDAY_FILE_ID, READ_KEYS, &hl_key1, mode, 0 )) == 0; mode = ISNEXT )
    {
      holidays[i] = pHolidays;
      holidays[i]->mon = pHolRec->mon;
      holidays[i]->day = pHolRec->day;
      holidays[i]->year = pHolRec->year;
      holidays[i] = pHolidays++;
      i++;
    }
  db( HOLIDAY_FILE_ID, CLOSE, &hl_key1, 0, 0 );
  return ( (char *)hol_shmptr );
}

char *build_time_calls(  )
{
  TC_STRUCT     *pTimeCall;
  TC_HNDL       tc_hndl;
  int           i = 0, done = 0;
  CALL_REC      callrec;
  ZONE_HNDL     hZone = HNDL_NULL;
  time_t        tmp_time, lead_time;
  

  for ( i = 0; i < FLEET_MAX; i++ )
    {
      callrec.fleet = fleet[i]->fleet_id;
      callrec.pri_status = 63;
      callrec.veh_nbr = 0;
      callrec.due_date_time = time(0);

      if ( db_read_key( CALL_FILE_ID, &callrec, &cl_key4, ISGTEQ ) != SUCCESS )
	{
	  if ( iserrno == EENDFILE || iserrno == ENOREC )
	    continue;
	  else
	    {
	      ERROR(fleet[i]->fleet_id,CALL_FILE,"error reading call rec in build_intervals");
	      continue;
	    }
	}
      done = 0;
      while ( !done )
	{
	  if ( ( callrec.fleet != fleet[i]->fleet_id ) || ( callrec.pri_status != 63 ) )
	    break;

	  if ( callrec.pckup_zone == 0 )
	    callrec.pckup_zone = WAKEUP_ZONE;

	  if (  Zone_get_hndl( callrec.pckup_zone, i, &hZone ) == FAIL )
	    {
	      ERROR(' ', "", "Could not get zone handle" );
	      if ( db_read_key( CALL_FILE_ID, &callrec, &cl_key4, ISNEXT ) != SUCCESS )
		{
		  if ( iserrno == EENDFILE || iserrno == ENOREC )
		    done = 1;
		  else
		    {
		      ERROR(fleet[i]->fleet_id,CALL_FILE,
			    "error reading call rec in build_intervals");
		      done = 1;
		      
		    }	      
		}
	    }
	  else
	    {
	      if ( fleet[i]->zone_ptr[callrec.pckup_zone] != NULL )
		{
		  tc_hndl = (TC_HNDL) malloc( sizeof(TC_STRUCT) );
		  ++num_tc;
		  pTimeCall = ( TC_STRUCT * ) tc_hndl;	      
		  memset((char *) pTimeCall, '\0', sizeof( TC_STRUCT ));
		  
		  TC_set_value((TC_HNDL)tc_hndl, TC_CALL_NBR, (HNDL)callrec.nbr);
		  TC_set_value((TC_HNDL)tc_hndl, TC_DUE_TIME, (HNDL)callrec.due_date_time);
		  TC_set_value((TC_HNDL)tc_hndl, TC_ZONE, (HNDL)callrec.pckup_zone);
		  TC_set_value((TC_HNDL)tc_hndl, TC_FLEET, (HNDL)i);
		  strncpy( pTimeCall->drv_attr, callrec.drv_attr, ATTR_MAX );
		  strncpy( pTimeCall->veh_attr, callrec.veh_attr, ATTR_MAX );

		  if ( callrec.extended_type[0] == WAKEUP )
		      lead_time = callrec.due_date_time - fleet[i]->wakeup_lead_time;
		  else
		    {
		      if (callrec.extended_type[0] == TYPEX)
			tmp_time = fleet[i]->zone_ptr[callrec.pckup_zone]->typex_lead_time;
		      else
			tmp_time =  fleet[i]->zone_ptr[callrec.pckup_zone]->pickup_lead_time; /* get zone lead */
		      if (0)
			{
			  lead_time = callrec.due_date_time - (time_t)get_new_lead(tmp_time, callrec.due_date_time, i, offset) -
			    ( fleet[i]->time_calls_lead_supp * 60 );
			}
		      else
			lead_time = callrec.due_date_time - tmp_time;
		    }
		  
		  TC_set_value((TC_HNDL)tc_hndl, TC_LEAD_TIME, (HNDL)lead_time );
#ifdef FOO
		  TC_debug( TC_DEBUG_LOAD, (TC_HNDL)tc_hndl );
#endif
		  Zone_tc_list_add( (ZONE_TC_LIST_HNDL) Zone_get_value( hZone, ZONE_TC_LIST ), pTimeCall );
		}
		  
	      if ( !done )
		{
		  if ( db_read_key( CALL_FILE_ID, &callrec, &cl_key4, ISNEXT ) != SUCCESS )
		    {
		      if ( iserrno == EENDFILE || iserrno == ENOREC )
			done = 1;
		      else
			{
			  ERROR(fleet[i]->fleet_id,CALL_FILE,
				"error reading call rec in build_intervals");
			  done = 1;
			  
			}
		    }
		}
	    }
	}
    }
  
  return(SUCCESS);
}

char *build_res_periods( pResPer )
     RES_PERIODS *pResPer;
{
  RES_PERIODS       *res_periods;
  RP_REC            *rp_rec;
  RP_DEF            *day_info;
  int        rc;
  int               i, mode;

  if ( pResPer == NULL )
    return( NULL );

  rp_rec = ( RP_REC * ) req_buf;
  offset->res_period = ( RES_PERIODS * ) pResPer;
  res_periods = ( RES_PERIODS * ) pResPer;

  if(db(RP_FILE_ID,OPEN,&rp_key1,ISINOUT | ISMANULOCK,0) < 0){		/*  open the file */
    ERROR(' ',RP_FILE,"build_res_periods - cannot open file\n");
    cleanup();
  }
  bzero( res_periods, RES_PERIOD_MEM_SIZE );

  for ( mode = ISFIRST; (rc = db( RP_FILE_ID, READ_KEYS, &rp_key1, mode, 0 )) == 0; mode = ISNEXT )
    {
      for ( i = 0; i < RP_DAYS_PER_WEEK; i++ )
	{
	  if ( rp_rec->days_of_week[i] == RP_PERIOD_APPLIES )
	    {
	      if ( RES_add_period( &res_periods[i], rp_rec->period_definition.start_time,
				   rp_rec->period_definition.end_time, rp_rec->period_definition.period,
				   rp_rec->period_definition.lead_time_skew ) != 0 )
		{
		  db( RP_FILE_ID, CLOSE, &rp_key1, 0, 0 );
		  return ( NULL );
		}
	    }
	}
    }
  db( RP_FILE_ID, CLOSE, &rp_key1, 0, 0 );
  return ( (char *) pResPer + RES_PERIOD_MEM_SIZE );
}

/****************************************************************************
build_menus - allocate space for menus and information
*****************************************************************************/
char *build_menus(menu_shmptr)
struct menus *menu_shmptr;
{
	struct cisam_mf *mf_ptr;
	struct menus *menu[MENU_MAX];
	register i;
	int mode;
	
#ifdef DEBUG
	if(offset->prattle >= LEVEL1){
		sprintf(trace_str,"build_menus : menu_shmptr = %x \n",menu_shmptr);
		TRACE(trace_str);
	}
#endif

	mf_ptr = (struct cisam_mf*)req_buf;
	offset->menu = menu_shmptr;						/* initialize pointer to menu block */

	if(db(MENU_FILE_ID,OPEN,&mf_key2,ISINOUT | ISMANULOCK,0) < 0){		/*  open the file */
		ERROR(' ',MENU_FILE,"build_menus - cannot open file\n");
		cleanup();
	}

	mf_ptr->command = 0;
	mode = ISGTEQ;

	for(i=0;i<MENU_MAX;i++){
		if( db(MENU_FILE_ID,READ_KEYS,&mf_key2,mode,0) < 0){			/* read in all the menu records */
			if( iserrno != EENDFILE ){
				ERROR(' ',MENU_FILE,"build_menus - can't find menu\n");
				cleanup();
			}
		}
		menu[i] = menu_shmptr++ ;
		mode = ISNEXT;
		fill_menu(menu[i],mf_ptr);					/* copy stuff from cisam struct to dispatch */
	}
	menus_used = i;								/* get total number of menus read */
	
	db(MENU_FILE_ID,CLOSE,&mf_key2,0,0);					/*  close the file */
	return((char *)menu_shmptr);						/* return the end of the menus */
}

#ifdef NAMERICA
char *build_channels( pChannels )
     CHANNEL_ENTRY   *pChannels;
{
  struct cisam_channel   *pChannelRec;
  CHANNEL_ENTRY          *channel_shmptr;
  int                    i, rc, mode;

  offset->channels = ( CHANNEL_ENTRY * ) pChannels;
  if ( pChannels == NULL )
    return( NULL );

  pChannelRec = ( struct cisam_channel * ) req_buf;
  channel_shmptr = ( CHANNEL_ENTRY * ) pChannels;
  
  if (db(CHANNEL_FILE_ID, OPEN, &channel_key1, ISINOUT|ISMANULOCK, 0) < 0){
    ERROR(' ', CHANNEL_FILE_ID, "build_channels - cannot open file\n");
    cleanup();
  }
  
  i = 0;
  bzero( channel_shmptr, CHANNEL_MEM_SIZE );
  pChannelRec->channel_id[0] = '\0';
  for ( mode = ISFIRST; ( rc = db( CHANNEL_FILE_ID, READ_KEYS, &channel_key1, mode, 0 )) == 0; mode = ISNEXT )
    {
      strncpy(channel_shmptr[i].channel_id, pChannelRec->channel_id, 2);
      channel_shmptr[i].channel_nbr = pChannelRec->channel_nbr;
      strncpy(channel_shmptr[i].channel_short_descript, pChannelRec->channel_short_descript, 4);
      strncpy(channel_shmptr[i].channel_descript, pChannelRec->channel_descript, 32);
      i++;
      ++pChannels;
    }
  db(CHANNEL_FILE_ID, CLOSE, &channel_key1, 0, 0);
  return( (char *)pChannels );
}

char *build_tlg( pTLG )
     TLG_ENTRY    *pTLG;
{
  struct cisam_tlg       *pTLGRec;
  TLG_ENTRY              *tlg_shmptr;
  int i, rc, mode;

  offset->tlg_groups = ( TLG_ENTRY * ) pTLG;

  if ( pTLG == NULL )
    return( NULL );

  pTLGRec = ( struct cisam_tlg * ) req_buf;
  tlg_shmptr = ( TLG_ENTRY * )pTLG;

  if (db(TLG_FILE_ID, OPEN, &tlg_key1, ISINOUT | ISMANULOCK, 0) < 0){
    ERROR(' ', TLG_FILE, "build_tlg - cannot open file\n");
    cleanup();
  }

  i = 0;
  bzero( tlg_shmptr, TLG_MEM_SIZE );
  pTLGRec->tlg_nbr = 0;
  for( mode = ISFIRST; (rc = db(TLG_FILE_ID, READ_KEYS, &tlg_key1, mode, 0)) == 0; mode = ISNEXT )
    {
     tlg_shmptr[i].lo_range = pTLGRec->tlg_lo_range;
     tlg_shmptr[i].hi_range = pTLGRec->tlg_hi_range;
     tlg_shmptr[i].fleet = pTLGRec->tlg_fleet;
     i++;
     ++pTLG;
   }
  db(TLG_FILE_ID, CLOSE, &tlg_key1, 0, 0);
  return ( (char *)pTLG );
}
#endif

/****************************************************************************
build_veh_driv - Read the vehicle information from disk in order of vehicle
number and fleet id. Allocate space for each vehicle being used. Place the
vehicle in both the rf id to vehicle pointer translation index and the
vehicle number to vehicle pointer translation index. Initialize the next
avail cab in zone pointers to be NULL for all vehicle records.
*****************************************************************************/
char *build_veh_driv(warm)
int warm;
{
	int mode=ISFIRST;			/* mode for db calls */
	struct veh_driv *veh_ptr;
	VEH_REC *cveh_ptr;		/* pointer to a record returned by cisam */
	static char tmp_str[10];
	short tmp_zone;
	int drv_id;
	int i;
	SORTAVLQ veh_array[VEH_DRIV_MAX];
	int veh_array_index = 0;
	SORTAVLQ *h,*t,*c,*k;
	short   last_zone;
	long    last_time;
	int   last_driver;
	unsigned long tmp_rf_id;
	int   rf_id;
	ZONE_HNDL zone_hndl;
	char  bigger_trace_str[300];
	double gps_x, gps_y;
	VEH_REC veh_rec;
	short   new_zone = 0;
	
	veh_used = 0;
	cveh_ptr = (struct cisam_vh*)req_buf;
	if (warm)
	  {
	    if ((h = (SORTAVLQ *) malloc(sizeof(SORTAVLQ))) == NULL)
	      {
		ERROR(' ', "", "build_veh_driv - couldn't allocate memory for vehicle sort list.");
		cleanup();
	      }
	    h->next = NULL;
	  }	

	while( db(VEHICLE_FILE_ID,READ_KEYS,&veh_key2,mode,0) >= 0)
	{		
	  if( cveh_ptr->fleet_nbr < 0 || cveh_ptr->fleet_nbr > FLEET_MAX -1)
	    {
	      sprintf(trace_str,"build_veh_driv - vehicle %d does not belong to valid fleet.",cveh_ptr->nbr);
	      ERROR(cveh_ptr->fleet,"",trace_str);
	      mode = ISNEXT;
	      continue;
	    }
	  if ((veh_ptr = (struct veh_driv *)malloc(sizeof(struct veh_driv))) == NULL)
	    {
	      ERROR(' ', "", "build_veh_driv - couldn't allocate memory for vehicle.");
	      cleanup();
	    }            	      
	  up_veh_idx(veh_ptr);			/* allocate and populate vehicle */
	  if ((warm) || (test_data))
	    {
	      CheckMsgQ();		/* Hey monitor Im not dead Im busy */
	      /* Save values needed for later due to the dreaded "req_buf" */
	      if ( test_data )
		{
		  last_zone = 101 + (( 133 - 101 ) * drand48());
		  cveh_ptr->last_zone_posted = last_zone;
		  cveh_ptr->status = VEH_WARM_STATUS_POST;
		  last_time = time(0);
		  last_driver = cveh_ptr->nbr;
		}
	      else
		{
		  last_zone = cveh_ptr->last_zone_posted;
		  last_time = cveh_ptr->last_time_posted;
		  last_driver = cveh_ptr->last_driver;
		}
	      
	      if ( (cveh_ptr->status!=VEH_WARM_STATUS_SIGNOFF) || ( test_data ) )
		{
		  strcpy(veh_array[veh_array_index].mobile_rf_id, cveh_ptr->mobile_rf_id);
		  veh_array[veh_array_index].last_driver = last_driver;
		  veh_array_index++;
		  
		  
		  if (cveh_ptr->last_zone_posted)
		    {
		      if ((c = (SORTAVLQ *) malloc(sizeof(SORTAVLQ))) == NULL)
			{
			  ERROR(' ', "", "build_veh_driv - couldn't allocate memory for vehicle sort list.");
			  k = h;
			  t = k->next;
			  while (t)
			    {
			      k = k->next;
			      free(t);
			      t = k;
			    }
			  cleanup();
		      } else
			{
			  c->status = cveh_ptr->status;
			  c->fleet = cveh_ptr->fleet_nbr;
			  c->nbr = cveh_ptr->nbr;
			  c->last_driver = last_driver;
			  strcpy(c->mobile_rf_id, cveh_ptr->mobile_rf_id);
			  if (Zone_get_hndl(last_zone, veh_ptr->fleet_nbr, &zone_hndl) == SUCCESS)
			    {
			      /* if zone record is returned from isam call */
			      c->last_zone_posted = last_zone;
			      c->last_time_posted = last_time;
			  } else
			    {
			      c->last_zone_posted = 0;
			      c->last_time_posted = 0;
			    }
			  c->next = NULL;
			  if (h == NULL)
			    h = c;
			  else
			    {
			      
			      k = h;
			      t = k->next;
			      while ((t) && (t->last_zone_posted < c->last_zone_posted))
				{
				  
				  k = t;
				  t = t->next;
				}
			      if (t)
				{
				  if (t->last_zone_posted == c->last_zone_posted)
				    {
				      while (((t) && (t->last_time_posted < c->last_time_posted))
					     && ((t->last_zone_posted == c->last_zone_posted)))
					{
					  k = t;
					  t = t->next;
					}
				    }
				}
			      c->next = k->next;
			      k->next = c;
			    }
			}
		      
		    }
		}
	    }	  
	  mode = ISNEXT;
        }
	veh_populate_lists();
	if ( (warm) || (test_data))
	  {
	    offset->status.ncp = 0;	/* turn ncp off */
	    for (i = 0; i < veh_array_index; i++)
	      {
		rf_id = (int) strtol(veh_array[i].mobile_rf_id, (char **) 0, 16);
		if ((veh_ptr = (struct veh_driv *) Fleet_veh_id_list_find(rf_id)) != NULL)
		  {
		    drv_id = veh_array[i].last_driver;
		    // don't validate driver ID card serial number		    
		    (void) Driver_initialize(drv_id, (VEH_HNDL) veh_ptr, (char)NULL);
		    Veh_set_state((VEH_HNDL) veh_ptr, VEH_SIGN_IN_STATE, (HNDL) drv_id);
		    Driver_set_state((DRIVER_HNDL) veh_ptr, DRIVER_SIGN_IN_STATE, (HNDL) drv_id);
		    (void) is_suspended(veh_ptr->mid, veh_ptr, EITHER, MSG);	/* DO THIS ?? */

		  }
	      }
	    CheckMsgQ();
	    k = h;
	    t = k->next;
	    k = t;
	    tmp_zone = 0;
	    strcpy(bigger_trace_str, "");
	    while (t)
	      {
		rf_id = (int) strtol(t->mobile_rf_id, (char **) 0, 16);
		if ((veh_ptr = (struct veh_driv *) Fleet_veh_id_list_find(rf_id)) != NULL)
		  {
#ifdef DEBUG
		    if (tmp_zone != t->last_zone_posted)
		      {
			strcat(bigger_trace_str,"\n");
			TRACE(bigger_trace_str);
			tmp_zone = t->last_zone_posted;
			sprintf(bigger_trace_str,"build_veh_driv : rebuilt zone %d - ",tmp_zone);
		      }
		    sprintf(tmp_str," %d",veh_ptr->veh_nbr);
		    if (t->status == VEH_WARM_STATUS_EPOST)
		      strcat(tmp_str,"*");
		    strcat(bigger_trace_str,tmp_str);
#endif
		    
		    if (t->status == VEH_WARM_STATUS_EPOST)
		      Veh_warm_start_epost((VEH_HNDL) veh_ptr, t->last_zone_posted);
		    else
		      Veh_set_state((VEH_HNDL) veh_ptr, VEH_POSTED_STATE, (HNDL) t->last_zone_posted);

		    if ( test_data )
		      {
			if ( veh_ptr->fleet == 'A' )
			  {
#ifdef FOO
			    gps_x = (double)((700977) + ((702132 - 700977) * drand48()));
			    gps_y = (double)((6148730) + ((6152580 - 6148730) * drand48()));
#endif
			    gps_x = (double)((-68736) + ((-104283 - -68736) * drand48()));
			    gps_y = (double)((136455) + ((177148 - 136455) * drand48()));
			    
			  }
			else if ( veh_ptr->fleet == 'B' )
			  {
			    gps_x = (double)((659361) + ((678588 - 659361) * drand48()));
			    gps_y = (double)((6148730) + ((6152580 - 6148730) * drand48()));
			  }
			veh_ptr->gps_state = VEH_GPS_STATE_IDLE;
			veh_ptr->gps_long = gps_x;
			veh_ptr->gps_lat = gps_y;
			Veh_set_value((VEH_HNDL) veh_ptr, VEH_GPS_TIME, (HNDL) time(0) );

			if ( veh_ptr->fleet == 'A' || veh_ptr->fleet == 'B' )
			  Tracking_Layer_db_add( (VEH_HNDL)veh_ptr );			    
			/* update database record */
			bzero(&veh_rec, sizeof(VEH_REC));
			veh_rec.fleet = veh_ptr->fleet;
			veh_rec.nbr = veh_ptr->veh_nbr;
			if ( db_read_key(VEHICLE_FILE_ID, &veh_rec, &veh_key2, ISEQUAL) == SUCCESS )
			  {
			    veh_rec.gps_lat = veh_ptr->gps_lat;
			    veh_rec.gps_long = veh_ptr->gps_long;
			    veh_rec.gps_state = VEH_GPS_STATE_IDLE;
			    db_update( VEHICLE_FILE_ID, &veh_rec );
			  }
			Veh_reset_state( (VEH_HNDL) veh_ptr, VEH_POSTED_STATE, HNDL_NULL, HNDL_NULL );
			new_zone = 0;
			GPS_get_best_zone((VEH_HNDL)veh_ptr, &new_zone);
			if ( new_zone > 0 )
			  Veh_set_state((VEH_HNDL) veh_ptr, VEH_POSTED_STATE, (HNDL) new_zone );


		      }		    

		    k = k->next;
		    free(t);
		    t = k;
		  }
	      }
	    offset->status.ncp = 1;	/* turn ncp on */
	    free(h);
	  }	
	veh_stats_inc = veh_used / DRV_STATS_FREQ;
	veh_stats_inc++;          /* for good measure */
	return(SUCCESS);
      }                               /* end build_veh_driv */

/****************************************************************************
up_veh_idx - update the rf and veh indices
*****************************************************************************/
up_veh_idx(veh_ptr)
struct veh_driv *veh_ptr;		/* pointer  to a vehicle record */
{
   VEH_REC *cveh_ptr;	/* pointer to a record returned by cisam */
   unsigned long tmp_rf_id;
   FLEET_VEH_NBR_LIST_HNDL veh_nbr_list;
   FLEET_HNDL fleet_hndl;

   cveh_ptr = (VEH_REC *) req_buf;

   memset((char *) veh_ptr, '\0', sizeof(struct veh_driv));

   Veh_rec_2_obj(veh_ptr, cveh_ptr, FALSE);	/* copy in cisam returned values */
   tmp_rf_id = (unsigned long) strtol(cveh_ptr->mobile_rf_id, (char **) 0, 16);
   veh_temp[veh_used] = veh_ptr;

   veh_used++;			/* increment the number of vehicles */
   offset->veh_used = veh_used;
   veh_ptr->time_info = 0;	/* initialize time field */
}



char *
build_except_count(exception_count_ptr)
     struct exception_count_struct *exception_count_ptr;
{
  exception_count = exception_count_ptr;
  offset->except_count = exception_count;
  return((char *) exception_count_ptr + sizeof(struct exception_count_struct));
}

/***************************************************************************
* make_scratch - initialize a scratch area 
*
* The size of the scratch area will be the size of the call index
* because its main function is to yield space for the shifting of
* the call index when calls are entered with call numbers out of sequence.
***************************************************************************/
make_scratch()
{
	if((scratch_ptr=(char *)malloc(120)) == NULL){	/* allocate memory for scratch */
		ERROR(' ',"","make_scratch - couldn't allocate memory for scratch.");
		cleanup();
	}
}


/****************************************************************************
* init	- build shared memory structures
* 
* Shared memory is allocated in one segment. This segment is broken down
* into various structures.  Each call to build a structure in shared memory
* is passed an address where free memory is available.  The call returns
* with the next available free memory address to be used on the subsequent
* call.
*****************************************************************************/
init(warm_start, prattle_level)
int warm_start;
int prattle_level;
{
	char *tmp_shmptr;		/* pointer to free shared memory */
	register i;


	if (db_open( TL_FILE_ID, ISINOUT | ISMANULOCK ) != SUCCESS )
	  {
	    ERROR(' ', "", "Could not open TRACKING LAYER table");
	    cleanup();
	  }
	
	tmp_shmptr= make_mem();						/* allocate shared memory */
	offset->prattle = prattle_level;				/* set prattle level to command line value */

	tmp_shmptr = build_calls((struct calls*)tmp_shmptr);		/* build the call structure */

	tmp_shmptr = build_zones((FLEET *)tmp_shmptr);		/* build the zone structures */

	build_veh_driv(warm_start);


	for ( i = 0; i < FLEET_MAX; i ++ )
	  build_inv_zones(i);

	CheckInWithMonitor();

	tmp_shmptr = build_users((struct users*)tmp_shmptr);		/* build the user structure */
	tmp_shmptr = build_except((struct excepts*)tmp_shmptr);		/* build the exeption structure */
	
	tmp_shmptr = build_except_desc((struct excepts*)tmp_shmptr);	/* build the exeption description structure */
	
	CheckInWithMonitor();
	tmp_shmptr = build_menus((struct menus*)tmp_shmptr);		 /* build the menu structure */

	tmp_shmptr = build_except_count((struct exception_count_struct *) tmp_shmptr);
	
	tmp_shmptr = build_res_periods( (RES_PERIODS *)  tmp_shmptr);

	tmp_shmptr = build_holidays( (HOLIDAYS *) tmp_shmptr );
	
	build_time_calls( );

#ifdef NAMERICA
	tmp_shmptr = build_tlg((TLG_ENTRY *) tmp_shmptr);
	tmp_shmptr = build_channels((CHANNEL_ENTRY *) tmp_shmptr);
#endif

	make_scratch();

	/* set file name for  files to be cut off at midnight and restarted */
	set_file_names();

	if(warm_start)
	  do_warm();							/* Do the warm start routine */


	if (0)  /* advanced credit card support */
	  {
	    if (db_open( RECEIPT_DATA_FILE_ID, ISINOUT | ISMANULOCK ) != SUCCESS )
	      cleanup();
	    if (db_open( CARD_TYPE_FILE_ID, ISINOUT | ISMANULOCK ) != SUCCESS )
	      cleanup();
	    if (db_open( BAD_CREDIT_FILE_ID, ISINOUT | ISMANULOCK ) != SUCCESS )
	      cleanup();	    
	  }

	if (0)
	  {
	    if ( db_open( VEH_CMSG_FILE_ID, ISINOUT | ISMANULOCK ) != SUCCESS )
	      {
		ERROR(' ',"", "Could not open VEH CANMSG table");
		cleanup();
	      }
	  }
	
	if (db( CALLMSG_FILE_ID, OPEN, &cmsg_key1, ISINOUT | ISMANULOCK, 0 ) < 0 )
	  {
	    cleanup();
	  }

	if (db( IMSI_FILE_ID, OPEN, &imsi_key1, ISINOUT | ISMANULOCK, 0 ) < 0 )
	  {
	  }



	/* Memory Management for Packet ID's */
	offset->mm_ul_cmd = MM_LOG_ON;
	
	return(0);
}

/****************************************************************************
* fill_menu - Copy data from the disk file structure to the internal
* structure.
*****************************************************************************/
fill_menu(dest,src)
struct menus *dest;
struct cisam_mf *src;
{

	dest->mf_nbr = src->nbr;
	dest->mf_command = src->command; 
	dest->mf_last_menu_item  = src->last_menu_item;
	dest->mf_type =  src->type;
	strcpy(dest->mf_desc, src->desc);    
	strcpy(dest->mf_unix_string,src->unix_string);
	dest->mf_prev_scr_flag = src->prev_scr_flag;
}

/*********************************************************************
* get_syscntl 
* 	Returns the record in the system control data base.
*********************************************************************/
int get_syscntl(sys_day_end)
int *sys_day_end;
{
	struct	cisam_syc *systemrec;
	systemrec=(struct cisam_syc*)req_buf;	/* to cast req_buf */

	if( db(SYSCNTL_FILE_ID,OPEN,NULL,ISINPUT | ISMANULOCK,0) < 0){	/* open CALL db for read only */
		ERROR(' ',SYSCNTL_FILE,"Error opening file");
		cleanup();
	}

	if( db_read(SYSCNTL_FILE_ID, req_buf, ISFIRST) != SUCCESS )
	  {
	    ERROR(' ',SYSCNTL_FILE,"Error reading file");
	    cleanup();
	  }

	*sys_day_end=systemrec->day_end;

	if(db_close(SYSCNTL_FILE_ID) != SUCCESS )
	  {
	    ERROR(' ',SYSCNTL_FILE,"Error closing file");
	  }
        return(SUCCESS);
}

RES_add_period(RES_PERIODS *res_periods, int start_time, int end_time, short period, float lead_time_skew)
{
	int rc = 0;
	RP_DEF *day_info;

	/* if day wrap, make another entry */
	if (end_time < start_time)
	{
		rc = RES_add_period(res_periods, 0, end_time, period, lead_time_skew);

		day_info = &res_periods->info[res_periods->end];	/* Next available slot. */
		day_info->end_time = 2359;
	}
	else
	{
		day_info = &res_periods->info[res_periods->end];	/* Next available slot. */
		day_info->end_time = end_time;
	}

	day_info->start_time = start_time;
	day_info->period = period;
	day_info->lead_time_skew = lead_time_skew;

	if (++res_periods->end >= RES_MAX_PERIODS)
		rc = -1;

	return(rc);
}

RET_STATUS
veh_populate_lists()
{
   double drand48();
   int   ii, index;
   FLEET_HNDL fleet_hndl;
   FLEET_VEH_NBR_LIST_HNDL veh_nbr_list;
   int   cnt = 0;
   struct veh_driv *veh_ptr, *tmp_veh_ptr;

   /* random placement */
   for (ii = 0; ii < (2 * veh_used); ii++)
   {
      index = veh_used * drand48();
      if (veh_temp[index] != NULL)
      {
	 (void) Fleet_get_hndl(BY_NBR, veh_temp[index]->fleet_nbr, &fleet_hndl);
	 veh_nbr_list = (FLEET_VEH_NBR_LIST_HNDL) Fleet_get_value(fleet_hndl, FLEET_VEH_NBR_LIST);
	 Fleet_veh_id_list_add((VEH_HNDL) veh_temp[index]);
	 Fleet_veh_nbr_list_add(veh_nbr_list, (VEH_HNDL) veh_temp[index]);
	 veh_temp[index] = NULL;
	 cnt++;
      }
   }

   if (cnt == veh_used)
      return (SUCCESS);

   /* linear for rest */
   for (ii = 0; ii < veh_used; ii += 2)
   {
      if (veh_temp[ii] != NULL)
      {
	 (void) Fleet_get_hndl(BY_NBR, veh_temp[ii]->fleet_nbr, &fleet_hndl);
	 veh_nbr_list = (FLEET_VEH_NBR_LIST_HNDL) Fleet_get_value(fleet_hndl, FLEET_VEH_NBR_LIST);
	 Fleet_veh_id_list_add((VEH_HNDL) veh_temp[ii]);
	 Fleet_veh_nbr_list_add(veh_nbr_list, (VEH_HNDL) veh_temp[ii]);
	 veh_temp[ii] = NULL;
      }
   }

   for (ii = veh_used - 1; ii >= 0; ii--)
   {
      if (veh_temp[ii] != NULL)
      {
	 (void) Fleet_get_hndl(BY_NBR, veh_temp[ii]->fleet_nbr, &fleet_hndl);
	 veh_nbr_list = (FLEET_VEH_NBR_LIST_HNDL) Fleet_get_value(fleet_hndl, FLEET_VEH_NBR_LIST);
	 Fleet_veh_id_list_add((VEH_HNDL) veh_temp[ii]);
	 Fleet_veh_nbr_list_add(veh_nbr_list, (VEH_HNDL) veh_temp[ii]);
	 veh_temp[ii] = NULL;
      }
   }
   
   return (SUCCESS);
}				/* end veh_populate_lists */

test_drive()
{
  FLEET_VEH_NBR_LIST_HNDL   veh_nbr_list;
  FLEET_HNDL                fleet_hndl;
  VEH_HNDL                  veh_hndl;
  VEH                       *veh_ptr;
  VEH_REC                   veh_rec;
  double                    gps_x, gps_y;

  Fleet_get_hndl( BY_NBR, 0, &fleet_hndl );
  //  srand48();
  veh_nbr_list = (FLEET_VEH_NBR_LIST_HNDL) Fleet_get_value( fleet_hndl, FLEET_VEH_NBR_LIST );
  Fleet_veh_nbr_list_each_get( veh_nbr_list, veh_hndl )
    {
      veh_ptr = ( VEH * )veh_hndl;
      veh_ptr->gps_long -= (250 * drand48());
      veh_ptr->gps_lat -= (125 * drand48());
      Tracking_Layer_db_add( (VEH_HNDL)veh_ptr );
      veh_rec.fleet = veh_ptr->fleet;
      veh_rec.nbr = veh_ptr->veh_nbr;
      if ( db_read_key(VEHICLE_FILE_ID, &veh_rec, &veh_key2, ISEQUAL) == SUCCESS )
	{
	  veh_rec.gps_lat = veh_ptr->gps_lat;
	  veh_rec.gps_long = veh_ptr->gps_long;
	  db_update( VEHICLE_FILE_ID, &veh_rec );
	}
      gps_msg_hndl( veh_ptr, MDT_MSG_GPS );
    } Fleet_veh_nbr_list_each_end
      
      (void) Timer_create(
			  TIMER_TYPE, T_TEST_DRIVE,
			  TIMER_EXPIRE_TIME, 60,
			  TIMER_NOTIFY_FUNC, test_drive,
			  ATTRIBUTE_END);      
}
  
  
