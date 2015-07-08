/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: bcheck_files.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1997/08/09 18:08:03 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/sysadmin/bcheck_files.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: bcheck_files.c,v 1.2 1997/08/09 18:08:03 taxiadm Exp $";

#include <stdio.h>
#include "../include/path.h"

#define MAX_TABLE_NAMES 39
static char *table_names[ MAX_TABLE_NAMES ] = { 0 };

init_array_of_table_names()
{
    table_names[0] = CALL_FILE;
    table_names[1] = CALLH_FILE;
    table_names[2] = DRIVER_FILE;
    table_names[3] = VEHICLE_FILE;
    table_names[4] = ZONE_FILE;   
    table_names[5] = FLEET_FILE;   
    table_names[6] = ADDRESS_FILE;
    table_names[7] = ACCOUNT_FILE;
    table_names[8] = ATTR_FILE;
    table_names[9] = EMERG_FILE;
    table_names[10] = ERROR_FILE;
    table_names[11] = EXCTYPE_FILE;
    table_names[12] = EXCGRP_FILE;
    table_names[13] = INTERST_FILE;
    table_names[14] = LOCALTY_FILE;
    table_names[15] = HELP_FILE;
    table_names[16] = HOLIDAY_FILE;
    table_names[17] = PLACENM_FILE;
    table_names[18] = MENU_FILE;
    table_names[19] = MAILBOX_FILE;
    table_names[20] = MSGLOG_FILE;
    table_names[21] = RATE_FILE;
    table_names[22] = RPTMSG_FILE;
    table_names[23] = SEAL_FILE;
    table_names[24] = STRNAME_FILE;
    table_names[25] = STRTYPE_FILE;
    table_names[26] = SUBCALL_FILE;
    table_names[27] = SUBZONE_FILE;
    table_names[28] = SYSCNTL_FILE;
    table_names[29] = UNZONE_FILE;
    table_names[30] = PROFILE_FILE;
    table_names[31] = CANMSG_FILE;
    table_names[32] = STATDRV_FILE;
    table_names[33] = STATUSR_FILE;
    table_names[34] = STATS_FILE;
    table_names[35] = S_SCHED_FILE;
    table_names[36] = S_SHIFT_FILE;
    table_names[37] = SHIFT_FILE;
    table_names[38] = SHIFTH_FILE;
     
}

/* 
 * Compare index (.idx) file to data (.dat) file to see
 * whether the two are consistent. If they are not, bcheck 
 * deletes and rebuilds the corrupted indexes.
 */

main()
{
	int index;
	char 	command[ 64 ];

	init_array_of_table_names();

	for (index = MAX_TABLE_NAMES - 1; index; index--)
    {
        sprintf( command,"bcheck -y %s", table_names[ index ] );
        system( command );
    } /* bcheck all the files */
}
