/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Fleet_lists_public.h,v $
*  @(#)  $Revision: 1.9 $
*  @(#)  $Date: 2004/06/22 17:10:04 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Fleet_lists_public.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#include "LL.h"
#define FLEET_VEH_ID_LIST_FIRST    LIST_FIRST
#define FLEET_VEH_ID_LIST_NEXT     LIST_NEXT
/* THERE IS ONE AND ONLY ONE FLEET_VEH_ID_LIST */
LinkedList * 	Fleet_veh_id_list_create();
RET_STATUS          	Fleet_veh_id_list_add(VEH_HNDL);
RET_STATUS          	Fleet_veh_id_list_remove(VEH_HNDL);
VEH_HNDL            	Fleet_veh_id_list_find( unsigned long);
RET_STATUS          	Fleet_veh_id_list_destroy();
VEH_HNDL             	Fleet_veh_id_list_get(short);
   
#define	Fleet_veh_id_list_each_get(veh_hndl)	\
   { LL_Rewind(fleet_veh_id_list); \
     do { veh_hndl = LL_Get(fleet_veh_id_list);
#define	Fleet_veh_id_list_each_end }while(LL_Next(fleet_veh_id_list)==0); LL_Rewind(fleet_veh_id_list); } 
#define FLEET_VEH_NBR_LIST_FIRST    LIST_FIRST
#define FLEET_VEH_NBR_LIST_NEXT     LIST_NEXT
LinkedList *        Fleet_veh_nbr_list_create();
RET_STATUS          Fleet_veh_nbr_list_add( LinkedList *, VEH_HNDL);
RET_STATUS          Fleet_veh_nbr_list_remove(LinkedList *,  VEH_HNDL);
VEH_HNDL            Fleet_veh_nbr_list_find(LinkedList *, short);
RET_STATUS          Fleet_veh_nbr_list_destroy(LinkedList *);
VEH_HNDL            Fleet_veh_nbr_list_get(FLEET_VEH_NBR_LIST_HNDL, short);
#define Fleet_veh_nbr_list_each_get(veh_nbr_list, veh_hndl)  \
   { LL_Rewind(veh_nbr_list);  \
     do { veh_hndl = LL_Get(veh_nbr_list);
  
#define	Fleet_veh_nbr_list_each_end  }while(LL_Next(veh_nbr_list)==0); LL_Rewind(veh_nbr_list); }
#define FLEET_DRIVER_VEH_LIST_FIRST    LIST_FIRST
#define FLEET_DRIVER_VEH_LIST_NEXT     LIST_NEXT
LinkedList *        Fleet_driver_veh_list_create();
RET_STATUS          Fleet_driver_veh_list_add( LinkedList *, VEH_HNDL);
RET_STATUS          Fleet_driver_veh_list_remove( LinkedList *,  VEH_HNDL);
VEH_HNDL            Fleet_driver_veh_list_find( LinkedList *, int);
RET_STATUS          Fleet_driver_veh_list_destroy( LinkedList *);
VEH_HNDL            Fleet_driver_veh_list_get(LinkedList *, short);
#define Fleet_driver_veh_list_each_get(driver_veh_list, veh_hndl)       \
   { VEH_HNDL _next; LL_Rewind(driver_veh_list); \
   for ((veh_hndl) = Fleet_driver_veh_list_get((driver_veh_list), FLEET_DRIVER_VEH_LIST_FIRST),\
        _next = Fleet_driver_veh_list_get((driver_veh_list), FLEET_DRIVER_VEH_LIST_NEXT);\
        (veh_hndl); \
        (veh_hndl) = _next, _next = Fleet_driver_veh_list_get((driver_veh_list), FLEET_DRIVER_VEH_LIST_NEXT)) {
#define Fleet_driver_veh_list_each_end(driver_veh_list) } LL_Rewind(driver_veh_list); }
#define FLEET_ZONE_CALLS_LIST_FIRST    LIST_FIRST
#define FLEET_ZONE_CALLS_LIST_NEXT     LIST_NEXT
LinkedList *                 Fleet_zone_calls_list_create();
RET_STATUS                   Fleet_zone_calls_list_add( LinkedList *, ZONE_HNDL);
RET_STATUS                   Fleet_zone_calls_list_remove(LinkedList *,  ZONE_HNDL);
RET_STATUS                   Fleet_zone_calls_list_destroy(LinkedList *);
ZONE_HNDL                    Fleet_zone_calls_list_get(LinkedList *, short);
#define Fleet_zone_calls_list_each_get(fleet_zone_calls_list, zone_hndl)        \
   { ZONE_HNDL _next; LL_Rewind(fleet_zone_calls_list); \
   for ((zone_hndl) = Fleet_zone_calls_list_get((fleet_zone_calls_list), FLEET_ZONE_CALLS_LIST_FIRST),\
        _next = Fleet_zone_calls_list_get((fleet_zone_calls_list), FLEET_ZONE_CALLS_LIST_NEXT);\
        (zone_hndl); \
        (zone_hndl) = _next, _next = Fleet_zone_calls_list_get((fleet_zone_calls_list), FLEET_ZONE_CALLS_LIST_NEXT)) {
#define Fleet_zone_calls_list_each_end(fleet_zone_calls_list) } LL_Rewind(fleet_zone_calls_list); }
#define FLEET_EXCPT_LIST_FIRST    LIST_FIRST
#define FLEET_EXCPT_LIST_NEXT     LIST_NEXT
#define FLEET_EXCPT_LIST_PREV     LIST_PREV
LinkedList *                 Fleet_excpt_list_create();
RET_STATUS                   Fleet_excpt_list_add( LinkedList *, EXCPT_HNDL);
RET_STATUS                   Fleet_excpt_list_remove(LinkedList *,  EXCPT_HNDL);
RET_STATUS                   Fleet_excpt_list_destroy(LinkedList *);
EXCPT_HNDL                   Fleet_excpt_list_get(LinkedList *, short);
EXCPT_HNDL                   Fleet_excpt_list_find(LinkedList *, CALL_HNDL);
   
#define	Fleet_excpt_list_each_get(fleet_excpt_list, excpt_hndl)	\
   { EXCPT_HNDL _next; LL_Rewind(fleet_excpt_list); \
   for ((excpt_hndl) = Fleet_excpt_list_get((fleet_excpt_list), FLEET_EXCPT_LIST_FIRST),\
	_next = Fleet_excpt_list_get((fleet_excpt_list), FLEET_EXCPT_LIST_NEXT);\
	(excpt_hndl); \
	(excpt_hndl) = _next, _next = Fleet_excpt_list_get((fleet_excpt_list), FLEET_EXCPT_LIST_NEXT)) {
#define	Fleet_excpt_list_each_end(fleet_excpt_list) } LL_Rewind(fleet_excpt_list); }
#define FLEET_SUSP_LIST_FIRST    LIST_FIRST
#define FLEET_SUSP_LIST_NEXT     LIST_NEXT
LinkedList *        Fleet_susp_veh_list_create();
RET_STATUS          Fleet_susp_veh_list_add( LinkedList *, VEH_HNDL);
RET_STATUS          Fleet_susp_veh_list_remove(LinkedList *,  VEH_HNDL);
RET_STATUS          Fleet_susp_veh_list_destroy(LinkedList *);
VEH_HNDL            Fleet_susp_veh_list_get(LinkedList *, short);
VEH_HNDL            Fleet_susp_veh_list_find(LinkedList *, short);
   
#define	Fleet_susp_veh_list_each_get(susp_list, veh_hndl)	\
   { VEH_HNDL _next; LL_Rewind(susp_list); \
   for ((veh_hndl) = Fleet_susp_veh_list_get((susp_list), FLEET_SUSP_LIST_FIRST),\
	_next = Fleet_susp_veh_list_get((susp_list), FLEET_SUSP_LIST_NEXT);\
	(veh_hndl); \
	(veh_hndl) = _next, _next = Fleet_susp_veh_list_get((susp_list), FLEET_SUSP_LIST_NEXT)) {
#define	Fleet_susp_veh_list_each_end(susp_list) } LL_Rewind(susp_list); }
LinkedList *        Fleet_susp_drv_list_create();
RET_STATUS          Fleet_susp_drv_list_add( LinkedList *, VEH_HNDL);
RET_STATUS          Fleet_susp_drv_list_remove(LinkedList *,  VEH_HNDL);
RET_STATUS          Fleet_susp_drv_list_destroy(LinkedList *);
VEH_HNDL            Fleet_susp_drv_list_get(LinkedList *, short);
VEH_HNDL            Fleet_susp_drv_list_find(LinkedList *, int);
   
#define	Fleet_susp_drv_list_each_get(susp_list, veh_hndl)	\
   { VEH_HNDL _next; LL_Rewind(susp_list); \
   for ((veh_hndl) = Fleet_susp_drv_list_get((susp_list), FLEET_SUSP_LIST_FIRST),\
	_next = Fleet_susp_drv_list_get((susp_list), FLEET_SUSP_LIST_NEXT);\
	(veh_hndl); \
	(veh_hndl) = _next, _next = Fleet_susp_drv_list_get((susp_list), FLEET_SUSP_LIST_NEXT)) {
#define	Fleet_susp_drv_list_each_end(susp_list) } LL_Rewind(susp_list); }
#define FLEET_BIG_CAR_TRIP_LIST_NEXT     LIST_NEXT
#define FLEET_BIG_CAR_TRIP_LIST_FIRST    LIST_FIRST
#define FLEET_BIG_CAR_TRIP_LIST_PREV     LIST_PREV
LinkedList *                 Fleet_big_car_trip_list_create();
RET_STATUS                   Fleet_big_car_trip_list_add( LinkedList *, CALL_HNDL );
RET_STATUS                   Fleet_big_car_trip_list_remove( LinkedList *, CALL_HNDL );
CALL_HNDL                    Fleet_big_car_trip_list_get( LinkedList *, short );
CALL_HNDL                    Fleet_big_car_trip_list_find( LinkedList *, short );
#define Fleet_big_car_trip_list_each_get( bct_list, call_hndl ) \
{ CALL_HNDL _next; LL_Rewind(bct_list); \
   for ((call_hndl) = Fleet_big_car_trip_list_get((bct_list), FLEET_BIG_CAR_TRIP_LIST_FIRST),\
       _next = Fleet_big_car_trip_list_get((bct_list), FLEET_BIG_CAR_TRIP_LIST_NEXT);\
       (call_hndl); \
       (call_hndl) = _next, _next = Fleet_big_car_trip_list_get((bct_list),FLEET_BIG_CAR_TRIP_LIST_NEXT)) {
#define Fleet_big_car_trip_list_each_end(bct_list) } LL_Rewind(bct_list); }
#define FLEET_PKT_ID_LIST_NEXT           LIST_NEXT
#define FLEET_PKT_ID_LIST_FIRST          LIST_FIRST
#define FLEET_PKT_ID_LIST_PREV           LIST_PREV
LinkedList *           Fleet_pkt_id_list_create();
RET_STATUS             Fleet_pkt_id_list_add( PKT_ID_HNDL );
RET_STATUS             Fleet_pkt_id_list_remove( PKT_ID_HNDL );
PKT_ID_HNDL            Fleet_pkt_id_list_get( short );
PKT_ID_HNDL            Fleet_pkt_id_list_find( LinkedList *, int );
#define Fleet_pkt_id_list_each_get(  pkt_id_hndl )    \
{ PKT_ID_HNDL _next; LL_Rewind(fleet_pkt_id_list); \
    for ((pkt_id_hndl) = Fleet_pkt_id_list_get(FLEET_PKT_ID_LIST_FIRST),\
	 _next = Fleet_pkt_id_list_get(FLEET_PKT_ID_LIST_NEXT);\
	 (pkt_id_hndl); \
	 (pkt_id_hndl) = _next, _next = Fleet_pkt_id_list_get(FLEET_PKT_ID_LIST_NEXT)) {
#define Fleet_pkt_id_list_each_end(fleet_pkt_id_list) } LL_Rewind(fleet_pkt_id_list); }
