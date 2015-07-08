static char sz__FILE__[]="@(#)filemgmt.c	10.1.1.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  filemgmt.c; Rel: 5.4.0.0; Get date: 3/26/90 at 09:45:32
 * @(#)  Last delta: 3/26/90 at 09:44:44
 * @(#)  SCCS File: /taxi/sccs/s.filemgmt.c
 *                                                                           *
 *****************************************************************************/


#define  DECLARING
#include "comm_strdefs.h"
#include "fmgmt_strdefs.h"
#include "rep_strdefs.h"
#include <stdio.h>
#include <time.h> 
#include <ctype.h>
#include "machdep.h" 
#include <sys/types.h>
#include "mads_types.h"
#include "disk_usage.h" 
#include "key.h"
#include "language.h"
#include "path.h"


#define FILE_MENU1_MAX	14
#define MAX_LINE_LEN	80

char *blanks="                         ";
struct	menutype { char line[ MAX_LINE_LEN ]; } file_menu1[ FILE_MENU1_MAX ]={ 0 };

#define arraysize (sizeof(file_menu1)/sizeof(struct menutype))

#define MAX_FLEETS	8
#define MAX_REPORTS	30 /* max number of reports a given fleet may have */
#define RPT_SIZE	10 /* The disk space required to store the rpt output file */
#define RPT_NAME	13 /* the length of the report file name ie Axx.mmddyy*/

#define ERRORSTR_MAX 	1
static char *errorstr[ ERRORSTR_MAX ] = { 0 };

#define FILEPATHS_MAX	37

/* the cisam index and data files */
static char *filepaths[ FILEPATHS_MAX ] = { 0 };		


#define filenames 37

struct rpt_days_ {
		char date[sizeof("mmddyy ")];			/* the date the report was run for */
	};
struct rpt_days_ rpt_days[MAX_RPT_DAYS];

struct rpt_names_ {
		char name[sizeof("/usr/taxi/reports/fxx.mmddyy")];
	};
struct rpt_names_ rpt_names[MAX_RPT_DAYS * MAX_REPORTS];

struct rpt_size_ {
		char size[RPT_SIZE];
		char name[RPT_NAME];
	};
struct rpt_size_ rpt_size[MAX_RPT_DAYS * MAX_REPORTS * MAX_FLEETS];
char reply[128];

/*
 * This function was added to handle internationalization
 */
void init_file_mgmt_arrays()
{
	strcpy( file_menu1[0].line, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_44, "  TAXI Administration"));
	strcpy( file_menu1[1].line, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_45, "     FILE MANAGEMENT"));
	strcpy( file_menu1[2].line, "");
	strcpy( file_menu1[3].line, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_46, "1  Backup the Taxi System files \n"));
	strcpy( file_menu1[4].line, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_47, "2  Backup the Taxi Report files \n"));
	strcpy( file_menu1[5].line, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_48, "3  Restore the Taxi System files \n"));
	strcpy( file_menu1[6].line, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_49, "4  Restore the Taxi Report files \n"));
	strcpy( file_menu1[7].line, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_50, "5  Rebuild Data Files     \n"));
	strcpy( file_menu1[8].line, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_55, "6  Load, Update or Restore Bad Credit Card Account File\n"));
	strcpy( file_menu1[9].line, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_56, "7  Back Up Bad Credit Card Account File\n"));
	strcpy( file_menu1[10].line, "");
	strcpy( file_menu1[11].line, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_51, "Enter 'e' to return to previous menu or exit."));
	strcpy( file_menu1[12].line, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_52, "Type a number followed by a carriage return"));
	strcpy( file_menu1[13].line, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_53, "to execute a selection: "));


	errorstr[ 0 ] = catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_54, "Reply NOT in range 1 to 7. Please repeat.");

	filepaths[0] = CALL_FILE;
	filepaths[1] = CALLH_FILE;
	filepaths[2] = DRIVER_FILE;
	filepaths[3] = VEHICLE_FILE;
	filepaths[4] = ZONE_FILE;
	filepaths[5] = FLEET_FILE;
	filepaths[6] = ADDRESS_FILE;
	filepaths[7] = ACCOUNT_FILE;
	filepaths[8] = ATTR_FILE;
	filepaths[9] = EMERG_FILE;
	filepaths[10] = ERROR_FILE;
	filepaths[11] = EXCTYPE_FILE;
	filepaths[12] = EXCGRP_FILE;
	filepaths[13] = INTERST_FILE;
	filepaths[14] = LOCALTY_FILE;
	filepaths[15] = HELP_FILE;
	filepaths[16] = HOLIDAY_FILE;
	filepaths[17] = PLACENM_FILE;
	filepaths[18] = MENU_FILE;
	filepaths[19] = MAILBOX_FILE;
	filepaths[20] = MSGLOG_FILE;
	filepaths[21] = RATE_FILE;
	filepaths[22] = RPTMSG_FILE;
	filepaths[23] = SEAL_FILE;
	filepaths[24] = STRNAME_FILE;
	filepaths[25] = STRTYPE_FILE;
	filepaths[26] = SUBCALL_FILE;
	filepaths[27] = SUBZONE_FILE;
	filepaths[28] = SYSCNTL_FILE;
	filepaths[29] = UNZONE_FILE;
	filepaths[30] = PROFILE_FILE;
	filepaths[31] = CANMSG_FILE;
	filepaths[32] = STATDRV_FILE;
	filepaths[33] = STATUSR_FILE;
	filepaths[34] = STATS_FILE;
	filepaths[35] = S_SCHED_FILE;
	filepaths[36] = S_SHIFT_FILE;

}

#ifdef FOO
#ifdef sysV68
#include <sys/statfs.h>         /* for the statfs call */
float per_disk_used()
{
        struct statfs *buf;
        char * pathbuf = ".."; /* Device which has the report files on it                              */
        float percent_used,tds,f_space;
        buf = (struct statfs *) malloc(sizeof(struct statfs));
        if (statfs(pathbuf,buf,sizeof(struct statfs),0) < 0 )    /* get current disk usage info */
                printf(catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_1, "ERROR in statfs\n"));
        tds=(buf->f_blocks);
        f_space=(buf->f_bfree);
        percent_used = (f_space/tds) * 100;
	return(percent_used);
} 
#endif 
 
#ifdef ultrix 
#include <sys/param.h>
#include <sys/mount.h>

per_disk_used() 
{
	struct fs_data buf;

        if(statfs(REPORT_DATA, &buf) < 0 )    /* get current disk usage info */
                printf(catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_1, "ERROR in statfs\n"));
	return((int)(((float)buf.fd_req.bfree / (float)buf.fd_req.btot) * 100.0));
}
#endif
#endif


/*====================================================================
= check_space_restore
=====================================================================*/

check_space_restore(d, amt)
char * d;
char   amt;
{
	char  cmd[100];			/* string used in the system calls for the cpio restore commands */
	FILE *fileptr;
	int  j,i; 			/* index into rpt_size struct */
	int  c=1;
	char * chptr1;			
	char * chptr2;
	int  cmp=1;			/* return value from string compare */
	int percent_used;		/* percentage of the disk which is in use by files */
	int  doff= sizeof("/usr/taxi/reports/")-1;


	/* how much space does the report take */
	i=0;
	j = (atoi(reply) - 1);
	chptr1 =(char *) &rpt_names[j].name[doff];
	while (cmp != 0) {
		 chptr2 = (char *) rpt_size[i].name;
		 cmp = strncmp(chptr2, chptr1, 10);	
		 i++;
	} /* while */

 	 /* well we can finaly restore the needed file(s) */
	strncpy(cmd,"cpio -imacB /usr/taxi/reports/",sizeof ("cpio -imacB /usr/taxi/reports/"));
	strncat(cmd,rpt_size[i-1].name,RPT_SIZE);
	strncat(cmd," < /dev/rmt0h 2>/dev/null",sizeof(" < /dev/rmt0h 2>/dev/null"));
	system(cmd);
	if (amt == '1'){		/* user is restoring only one report */		
	  clear_scrn();
	  printf("\n%s%s %s %s.",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_5, "The file"),
		 rpt_size[i-1].name, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_6, "was restored") );
	  printf("\n%s%s. ",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_4,
					    "Enter 'e' to return to previous menu or exit") );
	  gets(reply); /* get reply */
	  return(0); 			/*return back to menu the rpt was restored */
	} /* if */
	else
	  printf("\n%s%s %s %s.",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_5, "The file"),
		 rpt_size[i-1].name, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_6, "was restored") );
	return(1);			/* allow more rpts to be restored because we are restoring one day */
	
}




/*====================================================================
= file_mgmt() -
=	Dispaly File Management Options.     
=	Who:	Bruce Young
=	What:	main() routine	When: 17 February 1988
=====================================================================*/
main()
{
	register int indx;	/* index for loops  */
	int arraylen=arraysize-1;
	int looping;
	int answer;
	int reply_ok;
	long current_time;
	char reply[128];		/* reply buffer */
        char nlspath[80];
        static char put_nlspath[80];
        int  putenv();	


	/*
	 * Set up for internationalization by setting the locale and opening the catalog
	 */
	strcpy(nlspath, "/usr/taxi/catalogs/finnish/%N.cat");
	sprintf(put_nlspath, "NLSPATH=%s", nlspath);
	putenv(put_nlspath);		
	setlocale(LC_ALL, "");
	open_catalog(&COMMON_m_catd, "common", 0, 0);
	open_catalog(&FMGMT_m_catd, "file_mgmt", 0, 0);
	open_catalog(&REP_catd, "da_report", 0, 0);

	init_file_mgmt_arrays();

	looping=1;
	while (looping) { /* loop until a deliberate exit */

		/* clear the screen */
		fflush(stdout);
		clear_scrn();

		/* draw the main menu */
		for (indx=0; indx<arraylen; indx++)
			printf("%s%s\n",blanks,file_menu1[indx].line);

		/* Prompt for a choice */
		printf("%s%s",blanks,file_menu1[arraylen].line);

		fflush(stdin);
		gets(reply); /* get reply */
		reply_ok=0;
	        switch (toupper(reply[0])) {

		case M_EXIT: /* return to the previous screen */
			looping=0;
			catclose( COMMON_m_catd );
			catclose( FMGMT_m_catd );
			catclose( REP_catd );
			break;

		case '1': /* a valid menu option */
			/* Make certain this choice is correct */
			printf("\n%s%s, \n",blanks,
			       catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_7, "If you want to backup the TAXI System files") );
			printf("%s%s ",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_8,
						       "insert the first tape cartridge and type Y and return when ready?") );
			fflush(stdin);
			gets(reply);
			if (!reply[0])
			    break;
			if (toupper(reply[0]) == YES) {	/* Backup Files  */
				printf("\n%s%s.",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_9,
								 "We are making a TAXI System backup, requiring several minutes"));
				printf("\n%s%s.", blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_10,
								  "You will be notified when done") );
				fflush(stdin);
				system("/usr/taxi/bin/taxi_backup");
				reply_ok=1;
				printf("\n%s%s.",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_11,
								 "The TAXI System backup is completed") );
				current_time = time((long *)0);
				printf("\n%s%s %s",
					blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_12,
							"Please remove your tape and label it with todays date"),
				       ctime(&current_time) );
				printf("\n%s%s",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_13,
								"Enter return to continue.... ") );
				fflush(stdin);
				gets(reply); /* get reply */
			}	/* end if backup */
			break;

		case '2':/* backup the report files onto tape */
			backup_rpt();
			break;

		case '3': 
			/* Make certain this choice is correct */
			printf("\n%s%s, \n",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_14,
							    "If you want to restore the TAXI files") );
			printf(  "%s%s ",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_15,
							 "then insert the first tape cartridge and type Y return when ready?") );
			fflush(stdin);
			gets(reply); /* get reply */
			if (!reply[0])
			    break;
			
			if (toupper(reply[0]) == YES) { /* Restore Files */
				printf("\n%s%s.",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_16,
								 "We are restoring your TAXI System. This requires several minutes"));
				printf("\n%s%s .",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_17,
								  "You will be notified when the system is restored") );
				fflush(stdin);
				system("/usr/taxi/bin/taxi_restore");
				reply_ok=1;
				printf("\n%s%s.",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_18,
								 "The TAXI System is restored") );
				printf("\n%s%s.",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_19,
								 "Please remove your tape") );
				printf("\n%s%s",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_13,
								"Enter return to continue.... ") );
				fflush(stdin);
				gets(reply); /* get reply */
			}	/* end if restore */
			break;

		case '4': /* restore the report files from tape */
			restore_rpt();
			break;

		case '5': 
			/* B-Check data files */
			answer=choose_tables();
			reply_ok=1;
			if (answer<0) 
				for (answer = filenames; answer; answer--) 
				  {
					sprintf(reply,"bcheck -y %s",filepaths[answer-1]);
					system(reply);
				  }	/* bcheck all the files */
			else if (answer>0) {
					sprintf(reply,"bcheck -y %s",filepaths[--answer]);
					system(reply);	
				}	/* bcheck one file */
			else reply_ok=0;
			break;
			
	      case '6':
			system("/usr/taxi/bin/credit -l");
			break;

	      case '7':
			system("/usr/taxi/bin/credit -b");
			break;

		default:
			printf("%s%s\n",blanks,errorstr[0]);
			reply_ok=1;
			break;
		} /* end switch */
		if (reply_ok) { 
			printf("%s%s",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_20, "Enter <CR> to continue.....") );
			gets(reply);
		}	/* end if */
	} /* end while 1 */
}  /* end main */

/*===================================================================
= backup_rpt()-
=
====================================================================*/
backup_rpt()
{
	char reply[128];		/* reply buffer */
	int reply_ok;	
	long current_time;
	int i =0; 			/* index into fleet_id */
	int goodfleets;
	char fleet_id[50];       	/* plenty of fleets */
	short fleet_num[9];
	char *fleet_name[50];
	char cmd[300];			/* string for system command */
	char tmp[3];

	printf("\n%s%s, \n",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_21,
					    "If you want to backup the seven days of TAXI report files") );
        printf(  "%s%s",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_22,
					"insert the first tape cartridge and type Y and return when ready? "));
        fflush(stdin);
        gets(reply); /* get reply */
        if (!reply[0] )
	    return;
	
	if (toupper(reply[0]) == YES) {   /* Backup Files  */
      		printf("\n%s%s.",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_23,
						 "We are making a TAXI Report backup, requiring several minutes") );
        	printf("\n%s%s.", blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_10, "You will be notified when done") );
        	fflush(stdin);
		goodfleets = choose_fleets(fleet_id,fleet_name,0,fleet_num);
		if (! goodfleets)
			printf("\n%s%s.\n",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_24,
							   "NO fleets are active so no reports will be backed up") );
		else {	
			system("cp /dev/null /usr/taxi/run/000.0");/*create null file for floowing loop*/
			while (fleet_id[i] != NULL){
		 		strcpy(cmd,"wc -c /usr/taxi/reports/");	/* the string should look like wc -c A*.* B*.* ... */	
				tmp[0]=fleet_id[i++];
				tmp[1]=NULL;
				strncat(cmd,tmp,1);
				strncat(cmd,"*.* /usr/taxi/reports/",sizeof("*.* /usr/taxi/reports/"));
				strncat(cmd,   "|cut -d'/' -f1,5 >>/usr/taxi/run/000.0 ",
					sizeof("|cut -d'/' -f1,5 >>/usr/taxi/run/000.0 "));
				system(cmd);
			}
         		system(". /usr/taxi/bin/taxi_rpt_bck"); 
       			reply_ok=1;
			printf("\n%s%s.",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_25, "The Report backup is completed") );
        		current_time = time((long *)0);
        		printf("\n%s%s: %s",
                                blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_26,
						"Please remove your tape and label it Taxi Reports and include todays date"),
				ctime(&current_time) );
		} /*else*/
       		printf("\n%s%s",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_13, "Enter return to continue.... ") );
        	fflush(stdin);
		gets(reply); /*alow the user to see the completed message */
	} /* if (strlen) */

}

/*===================================================================
= restore_rpt()-
=
====================================================================*/
restore_rpt()
{
	char reply[128];		/* reply buffer */

        printf("\n%s%s, \n",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_27, "If you want to restore some Report files") );
        printf(  "%s%s",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_28,
					"then insert the first Report tape cartridge and type Y return when ready? ") );
        fflush(stdin);
        gets(reply); /* get reply */
	if (toupper(reply[0]) == YES) {
		get_which_day_to_restore();
    	}
	
} /* restore_rpt*/

/*===================================================================
=get_rpt_size()
=
===================================================================*/
get_rpt_size()

{
	FILE *fileptr;
	int  j,i; 			/* index into rpt_size struct */
	int  c=1;
	int  cmp=1;			/* return value from string compare */
	int  doff= sizeof("/usr/taxi/reports/")-1;

 /* see if the disk has space for what the user whats to restore */
	 /* first see how big the file which are being restored are      */
	fileptr = fopen("/usr/taxi/reports/000.0","r");
	i = j =0;
	while ((c != EOF) && (i <= (MAX_RPT_DAYS * MAX_RPT_DAYS * MAX_FLEETS))){
	     c = getc(fileptr);
	     while ((c == ' ') && (c != EOF )) c = getc(fileptr);
	     rpt_size[i].size[j++] = c;
	     while ((c !='/') && ( c != EOF ) && (j <=  RPT_SIZE )){
		c = getc(fileptr);
		rpt_size[i].size[j++]=c;
	     } /* while */
	     rpt_size[i].size[j-1]=NULL;
	     j = 0;
	     while (( c != '\n') && ( c !='\r') && ( c != EOF ) &&  (j < RPT_NAME)){
		c = getc(fileptr);
	        rpt_size[i].name[j++]=c;
	     } /* while*/
	     rpt_size[i].name[j-1]=NULL;
	     i++;
	     j=0;
	} /* while */
}


/*===================================================================
=get_which_day_to_restore();
=
====================================================================*/
get_which_day_to_restore()
{
	int reply_ok;	
	char *stptr;
	long current_time;
	FILE *fileptr;
	int  c = 1;			/* get into the while loop first time */
	int  i=0;
	int  tmp;
	int  restored = 1;
	int  k=0;
	int  num_days =0;
	int  line_num = 0;		/* which line of the screen are we on */
	int  low_el = 0;
	int  j=0;			/* i is the # of rpt days and j is the counter for mmddyy */
	int  disply_line;
	int  cmp = -1;
	int  doff= sizeof("/usr/taxi/reports/fxx.") - 1;

	printf("\n%s%s.",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_29, "Checking which days are on the tape please wait") ); 
        fflush(stdin);
	system("cpio -iacBu '/usr/taxi/reports/0*.0' < /dev/rmt0h 2>/dev/null");  	/* redirect all msg*/   
	fileptr = fopen("/usr/taxi/reports/0.0","r");
	while ((c != EOF) && (i <= MAX_RPT_DAYS )){
	     c = getc(fileptr);
	     if ((c == '\n')||(c == '\r')){
                     rpt_days[i].date[6]='\0';
		     j=0;
                     i++;
             } /* if  */
	     else
	             rpt_days[i].date[j++]=c;
	}/* while */

	reply[0]= M_EXIT;
	num_days= i; 						/* save the number of lines read from file */
	while (!((atoi(reply)>0) && (atoi(reply)<=num_days))) {
		clear_scrn();
       		printf("\n%s%s.\n",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_30,
						   "These are the reporting days on the tape") );
       		for(i=1; i <= num_days;i++) printf("\n%s%d) %s",blanks,i,rpt_days[i-1].date);
		printf("\n\n%s%s.",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_4,
						   "Enter 'e' to return to previous menu or exit") );
		printf("\n%s%s",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_31,
						"Type a line number followed by a carriage return") );
		printf("\n%s%s: ",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_32,
						  "to view the reports for the selected day") );
	        fflush(stdin);
      	 	gets(reply); /* get reply */
       		if (!reply[0])
		    return;
		if (toupper(reply[0]) == M_EXIT)
		    return;
	}
	j=i=0;  /*adjust counters for next file read */
	c=1;
	fileptr = fopen("/usr/taxi/reports/00.0","r");
	while ((c != EOF) && (i <= MAX_REPORTS * MAX_RPT_DAYS)){
	     c = getc(fileptr); 
       	     if ((c == '\n')||(c == '\r')){ 
		rpt_names[i].name[sizeof("/usr/taxi/reports/fxx.mmddyy")+1]=NULL;
		j=0;
                i++;
	     } /* if*/
	     else {
		rpt_names[i].name[j++]=c;
		rpt_names[i].name[j]=NULL;
	    }
	} /*while*/
	/* display only those reports for the user inputted day */
	k = atoi(reply)-1;	
badinput:
	i=0;		
	cmp = 1;
	j = k;	
	clear_scrn();
       	printf("\n%s%s.",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_33,
					 "These are the reports on the tape for the selected day") );
	while ((cmp != 0 ) && (i <= MAX_RPT_DAYS * MAX_REPORTS)){              /* set i to the correct element in the array */
		stptr = (char *) &rpt_names[i++].name[doff];
		cmp = strncmp(stptr, rpt_days[j].date,5);	
	}
	line_num=0;
	disply_line=1;
	i--;
	low_el = i; 
	if (cmp == 0) {                                                       /* where any reports saved that day ? */
		while (strncmp(rpt_names[i].name+doff,rpt_days[j].date,6)==0){ /* display the rpts for the given day */
		    if (line_num >= 20) {
			printf("\n\n%s%s. ",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_34,
							    "Type a carriage return to continue viewing the report names"));
			gets(reply);
			line_num = 0;
			clear_scrn();
		    }/* if */ 
		    else{
			line_num++;  
	   	    	printf("\n%s%d) %s",blanks,disply_line++, rpt_names[i++].name);
		    } /*else*/
		}/*while*/
	}/*if */
	else 
		printf("\n %s%s.",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_35, "No reports found on the tape for that day") );

	printf("\n\n%s. ",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_4, "Enter 'e' to return to previous menu or exit"));
	printf("\n%s%s",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_31, "Type a line number followed by a carriage return"));
	printf("\n%s%s: ",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_36,
					  "to restore the report or a * to restore all reports for the selected day") );
	gets(reply); /* get reply */
	if (!reply[0] || toupper(reply[0]) == M_EXIT)
	    return;
	if ( ((atoi(reply) < 0) || (atoi(reply) > disply_line )) && (reply[0] != '*')) 
	   goto badinput;
	else{
	  get_rpt_size();	
	  if ( reply[0] != '*'){		/* Does the user want to restore more then one report?*/
	     tmp = (atoi(reply) + low_el);	/* no: so just restore the given report  */
	      clear_scrn();
              printf("\n%s%s.\n ",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_37, "Reading tape please wait") );
	     sprintf(reply,"%d",tmp);	
	     check_space_restore(rpt_days[j].date,'1');                 
	  }/*if*/
	  else{								/* yes: so search for all reports for the given day */
		/* move all the rpt names one by one into the reply string and call check_space_restore */	
		i=0;
		clear_scrn();
		printf("\n%s%s.\n ",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_37, "Reading tape please wait") );
		while (( i<= MAX_RPT_DAYS*MAX_REPORTS) && (restored == 1 )){
			stptr = (char *) &rpt_names[i++].name[doff];
			cmp = strncmp(stptr, rpt_days[j].date,5);       /* ??? check the 5 */
			if (cmp == 0) {
				sprintf(reply,"%d",i);
				restored = check_space_restore(rpt_days[j].date,'*');	/* restore all rpts for a given day */ 
			} /*if*/
		} /* while*/
		printf("\n%s%s. ",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_4,
						  "Enter 'e' to return to previous menu or exit") );
                gets(reply); /* get reply */
		system("rm /usr/taxi/reports/0*.0 2>");  		/* delete the three files which were restored */
                return(1);                      /*return back to menu the rpt was restored */
	  } /* else */
	}/*else*/	
}

/*====================================================================
= choose_tables() -
=	Display cisam tables and prompt for those tables to bcheck.
=====================================================================*/
choose_tables()
{
	char systembuff[64];
	char reply[60];		/* reply buffer */
	int bufflen, choice;

	sprintf(systembuff,"cat %s ",DB_FILENAME_FILE);

	while (1) { /* loop until a deliberate exit */

		/* clear the screen */
		clear_scrn();
		fflush(stdout);
		printf("\n%s\n\n", catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_38, "               THESE FILES ARE MAINTAINED"));
		system(systembuff);

		printf("\n%s%s 1..%d ,\n",blanks,catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_39, "Type a file number"), filenames,
		       catgets(FMGMT_m_catd, FMGMT_1, FMGMT_40, "to rebuild a particular file") );
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
		if ((choice=atoi(reply)) > 0 && choice <= filenames)
		    return(choice);

		printf("\n%s%s.  ",blanks, catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_42, "That reply was NOT valid") );
		printf("%s", catgets(FMGMT_m_catd, FMGMT_SET, FMGMT_43, "<CR> to continue .... ") );
		fflush(stdin);
		gets(reply);
	}	/* end while */

}	/* end choose_files */


