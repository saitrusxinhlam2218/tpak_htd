/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: sysmain.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1997/08/09 18:08:19 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/sysadmin/sysmain.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: sysmain.c,v 1.2 1997/08/09 18:08:19 taxiadm Exp $";
#define  DECLARING
#include "comm_strdefs.h"
#include "smain_strdefs.h"

#include <stdio.h>
#include "machdep.h"


#define MAX_LINE_LEN	80
#define MENU_0_MAX	12

static char *blanks="                         ";
static struct	menutype { char line[MAX_LINE_LEN]; } menu_0[MENU_0_MAX]={0};

#define options 5
#define root	0
#define arraysize (sizeof(menu_0)/sizeof(struct menutype))
#define informix_dir	"/usr/informix"


init_sys_main_arrays()
{	
	strcpy( menu_0[0].line, catgets(SMAIN_m_catd, SMAIN_SET, SMAIN_2, "  TAXI System Administration"));
	strcpy( menu_0[1].line, catgets(SMAIN_m_catd, SMAIN_SET, SMAIN_3, "       MAIN MENU"));
	strcpy( menu_0[2].line, "");
	strcpy( menu_0[3].line, catgets(SMAIN_m_catd, SMAIN_SET, SMAIN_4, "1  Start/Stop TAXI System \n"));
	strcpy( menu_0[4].line, catgets(SMAIN_m_catd, SMAIN_SET, SMAIN_5, "2  File System Maintenance\n"));
	strcpy( menu_0[5].line, catgets(SMAIN_m_catd, SMAIN_SET, SMAIN_6, "3  TAXI System Administration Functions\n"));
	strcpy( menu_0[6].line, catgets(SMAIN_m_catd, SMAIN_SET, SMAIN_7, "4  User Accounts \n"));
/*
	strcpy( menu_0[7].line, catgets(SMAIN_m_catd, SMAIN_SET, SMAIN_8, "5  Display Processes\n"));
*/
	strcpy( menu_0[7].line, "");
	strcpy( menu_0[8].line, "");
	strcpy( menu_0[9].line, catgets(SMAIN_m_catd, SMAIN_SET, SMAIN_10, "Enter \'e\' to return to previous menu or exit."));
	strcpy( menu_0[10].line, catgets(SMAIN_m_catd, SMAIN_SET, SMAIN_11, "Type a number followed by a carriage return"));
	strcpy( menu_0[11].line, catgets(SMAIN_m_catd, SMAIN_SET, SMAIN_12, "to execute a selection: "));

}

/*====================================================================
= system_main() -
=	System Administration Main Program. 
=	Who:	Bruce Young
=	What:	Written		When: 20 April 1988
=====================================================================*/
main()
{
	register int indx;	/* index for loops  */
	int arraylen=arraysize-1;
	int looping;

	char reply[16];		/* reply buffer */
	char systembuff[132];	/* system call buffer */
	char *informixdir;	/* directory for informix commands */
	extern char *getenv();
	char *term;
        char nlspath[80];
        static char put_nlspath[80];
        int  putenv();	



	if ((term = getenv("TERM")) != NULL) {

	    /* If we're on a vt100 (or emulation of one)
	       make it 132 columns. */
	    if (strcmp(term, "vt100") == 0 ||
		strcmp(term, "vt100w") == 0 ||
		strcmp(term, "vt100-w") == 0) {
		
		printf("\033[?3h");
		fflush(stdout);
	    }
	}
		

	/*
	 * Set up for internationalization by setting the locale and opening the catalog
	 */
	strcpy(nlspath, "/usr/taxi/catalogs/finnish/%N.cat");
	sprintf(put_nlspath, "NLSPATH=%s", nlspath);
	putenv(put_nlspath);		
	setlocale(LC_ALL, "");
	open_catalog(&COMMON_m_catd, "common", 0, 0);
	open_catalog(&SMAIN_m_catd, "system_main", 0, 0);


  	setuid(root);	/* the system_main program runs as root for required privledges */
  	setgid(root);	
	if (! (informixdir=getenv("INFORMIXDIR")) ) informixdir=informix_dir;

	init_sys_main_arrays();

	looping=1;
	while (looping) { /* loop until a deliberate exit */

		/* clear the screen */
		clear_scrn();

		/* draw the main menu */
		for (indx=0; indx<arraylen; indx++)
			printf("%s%s\n",blanks,menu_0[indx].line);

		/* Prompt for a choice */
		printf("%s%s",blanks,menu_0[arraylen].line);

		fflush(stdin); gets(reply); /* get reply */
		if (strlen(reply)) /* ignore <cr> */ switch (reply[0]) {
		case 'E': /* exit fromt his menu */
		case 'e': 
			looping=0;
			catclose( COMMON_m_catd );
			catclose( SMAIN_m_catd );
			break;
		case '1': /* a valid menu option */
			system("/usr/taxi/bin/system_mgmt");
			break;
		case '2': 
			system("/usr/taxi/bin/file_mgmt");
			break;
		case '3': 
			sprintf(systembuff,"%s/bin/isql mads -u main",informixdir);
			system(systembuff);
			break;
		case '4': 
			system("/usr/taxi/bin/admin_mgmt");
			break;
/*
		case '5': 
			system("/usr/taxi/bin/display_ps -1 2> /dev/null");
			printf("\n\n%s%s",blanks, catgets(SMAIN_m_catd, SMAIN_SET, SMAIN_1, "Type <CR> to continue.....") );
			fflush(stdin);
			gets(reply);
			break;
*/
		default:
			printf("%s%s\n",blanks, catgets(SMAIN_m_catd, SMAIN_SET, SMAIN_13, "Reply NOT in range 1 to 4. Please repeat."));
			printf("%s%s",blanks, catgets(SMAIN_m_catd, SMAIN_SET, SMAIN_1, "Type <CR> to continue.....") );
			fflush(stdin);
			gets(reply);
			break;
		} /* end switch */
	} /* end while 1 */
	clear_scrn();
}  /* end main */
