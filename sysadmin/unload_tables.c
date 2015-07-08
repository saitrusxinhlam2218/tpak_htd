/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: unload_tables.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1997/08/09 18:08:24 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/sysadmin/unload_tables.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: unload_tables.c,v 1.2 1997/08/09 18:08:24 taxiadm Exp $";

/*
* This program displays a screen of database tables that the user may dump	
* with the 'unload' command. The user selects the number of the file to	
* unload or '*' for all of the files. This program calls a shell script 
* called unload_db_files and passes the table to unload. Unload_db_files
* creates a temporary file that contains the unload command to execute. 
* The file is then executed with the command 'isql mads filename'. The 
* directory to unload the file to and others are set up in the shell script.
*/

#define  DECLARING
#include "comm_strdefs.h"
#include "fmgmt_strdefs.h"
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include "mads_types.h"
#include "key.h"
#include "language.h"
#include "path.h"

#define TABLENAMES_MAX   39
static char *tablenames[ TABLENAMES_MAX ] = { 0 };
#define ERRORSTR_MAX    1
static char *errorstr[ ERRORSTR_MAX ] = { 0 };
#define DB_FILES_SCREEN_MAX	1
static char *db_file_screen[ DB_FILES_SCREEN_MAX ] = { 0 };
char *blanks="              ";
#define db_dir  "/usr/taxi/mads.dbs"

init_arrays()
{
	tablenames[0] = "account";
	tablenames[1] = "address";
	tablenames[2] = "attr";
	tablenames[3] = "calls";
	tablenames[4] = "callh";
	tablenames[5] = "canmsg";
	tablenames[6] = "driver";
	tablenames[7] = "statdrv";
	tablenames[8] = "emerg";
	tablenames[9] = "error";
	tablenames[10] = "exctype";
	tablenames[11] = "excgrp";
	tablenames[12] = "fleet";
	tablenames[13] = "help";
	tablenames[14] = "holiday";
	tablenames[15] = "interst";
	tablenames[16] = "localty";
	tablenames[17] = "mailbox";
	tablenames[18] = "menu";
	tablenames[19] = "msglog";
	tablenames[20] = "placenm";
	tablenames[21] = "rate";
	tablenames[22] = "rptmsg";
	tablenames[23] = "seal";
	tablenames[24] = "s_sched";
	tablenames[25] = "s_shift";
	tablenames[26] = "stats";
	tablenames[27] = "strname";
	tablenames[28] = "strtype";
	tablenames[29] = "subcall";
	tablenames[30] = "subzone";
	tablenames[31] = "syscntl";
	tablenames[32] = "sysmenus";
	tablenames[33] = "sysmenuitems";
	tablenames[34] = "unzoned";
	tablenames[35] = "profile";
	tablenames[36] = "statusr";
	tablenames[37] = "vehicle";
	tablenames[38] = "zone";
}

main()
{
	int looping = 1, accessible;
	int table_to_unload;
	char command[256];        /* command buffer */

	/*
	Set up for internationalization by setting the locale and opening the catalog
	*/
	setlocale(LC_ALL, "");
	open_catalog(&COMMON_m_catd, "common", 0, 0);
	open_catalog(&FMGMT_m_catd, "file_mgmt", 0, 0);
	init_arrays();


	while ( looping )
	{
		table_to_unload = choose_tables();
		
		if (table_to_unload < 0)		/* unload all tables	*/
		{
			for (table_to_unload = TABLENAMES_MAX - 1; table_to_unload >= 0; table_to_unload--)
			{
				sprintf( command, "%s %s\n", UNLOAD_FILES, tablenames[table_to_unload] );
				system( command );
				sleep(3);	/* allow user to see rows unloaded  */
			}
		} /* unload all the tables */
		else 
			if (table_to_unload > 0) 	/* unload specified table 	*/
			{
				sprintf( command, "%s %s\n", UNLOAD_FILES, tablenames[table_to_unload-1] );
				system( command );
				sleep(3);	/* allow user to see rows unloaded  */
			}
			else 	/* user chose to exit */
			{
				looping = 0;
				continue;
			}
	
	}	/* while looping	*/

	/* close catalogs */
	catclose( COMMON_m_catd );
	catclose( FMGMT_m_catd );


}

choose_tables()
{
	char systembuff[64];
	char reply[60];     /* reply buffer */
	int choice;

	/* set up command to list the database files that are available to unload */
	sprintf(systembuff,"cat %s ",DB_FILENAMES_SCREEN);

	while (1) 
	{ /* loop until a deliberate exit */
							/* clear the screen */
		clear_scrn();
		fflush(stdout);
		printf("\n%s\n\n", catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_38, "               THESE FILES ARE MAINTAINED"));
		/* display screen of filenames for the user to choose from */
		system(systembuff);
		printf("\n%s%s %s 1..%d\n",blanks,catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_39, "Type a file number"),
			catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_53, "to execute a selection: "), TABLENAMES_MAX );
		printf("%s%s: ",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_41,
		"Enter 'e' to return to previous menu or * for all files") );
		fflush(stdout);
		fflush(stdin);
		gets(reply);

		if (!reply[0])
			continue; /* ignore <cr> */

		if (toupper(reply[0]) == M_EXIT)
			return(0);

		if (reply[0] == '*')
			return(-1);

		if ((choice=atoi(reply)) > 0 && choice <= TABLENAMES_MAX )
			return(choice);

		printf("\n%s%s.  ",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_42, "That reply was NOT valid") );
		printf("%s", catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_43, "<CR> to continue .... ") );
		fflush(stdin);
		gets(reply);
	}   /* end while */
}   /* end choose_files */

