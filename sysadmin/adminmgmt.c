static char rcsid[] = "$Id: adminmgmt.c,v 1.5 1998/10/16 18:15:40 taxiadm Exp $";

#define EOS             '\0'        /* This is the last char in ASCII string */
#define USERNAME_MAX    22          /* login user name maximum length */
#define DECLARING

#include "comm_strdefs.h"
#include "amgmt_strdefs.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>              /* for open */
#include <ctype.h>
#include <time.h>
#include <isam.h>
#include <errno.h>              /* for ENOENT */
#include <locale.h>
#include <langinfo.h>
#include "machdep.h"
#include "userpwd.h"
#include "language.h"
#include "mads_types.h"
#include "path.h"
#include "key.h"
#include "mad_error.h"

extern char getret();

struct offsets *offset;
struct stat fstat_buf;  /* ptr to file status structure buffer */

#define ADMIN_ACCOUNT_MAX       3
/* related static structures */
static  int     admin_group[] = {CALLGROUP, DISPGROUP, MANGROUP};
static  char    *admin_account[ ADMIN_ACCOUNT_MAX ]={ 0 };

/* To verify file existence and permissions for access */
#define MAX_LINE_LEN	80
#define FEXIST	0
#define MENU_1_MAX	10
#define MENU_111_MAX	8
#define ERRORSTR_MAX	2
#define MENU_12_MAX	6
#define MENU_11_MAX	10
#define MENU_113_MAX	6
#define MENU_14_MAX	5

char *admin_field();

static char *blanks="                         ";

static struct	menu1 { char line[MAX_LINE_LEN]; } menu_1[ MENU_1_MAX ] = { 0 };

#define options1 4
#define arraysize1 (sizeof(menu_1)/sizeof(struct menu1))

static struct	menu111 { char line[MAX_LINE_LEN]; } menu_111[ MENU_111_MAX ] = { 0 };

static char *errorstr[ ERRORSTR_MAX ] = { 0 };

#define options2 3
#define anotheraccount 7

static struct	menu12 { char line[MAX_LINE_LEN]; } menu_12[ MENU_12_MAX ] = { 0 };

static struct	menu11 { char line[MAX_LINE_LEN]; } menu_11[ MENU_11_MAX ] = { 0 };

#define header 3
#define options3 3
#define arraysize11 (sizeof(menu_11)/sizeof(struct menu11))

static struct	menu113 { char line[MAX_LINE_LEN]; } menu_113[ MENU_113_MAX ] = { 0 };

static struct	menu14 { char line[MAX_LINE_LEN]; } menu_14[ MENU_14_MAX ] = { 0 }; 

#define titlelen 3
#define headerline 4
#define pagelimit 17
#define	COLON	':'
#define	COMMA	','
#define	SHELL	"/bin/sh"
#define	up_differential 907
#define	up_removal 910


/*	Array initialization had to be changed for 
 *	internationalization. Note the line length is currently 80.
 */
menus_init()
{
	admin_account[0] = m_catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_36, "CALLER");
	admin_account[1] = m_catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_37, "DISPTCHR");
	admin_account[2] = m_catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_38, "MANAGER");

	strcpy( menu_1[0].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_39, "  TAXI Administration"));
	strcpy( menu_1[1].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_40, "     ACCOUNTS MENU"));
	strcpy( menu_1[2].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_41, "1  Add New Accounts       \n"));
	strcpy( menu_1[3].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_42, "2  Browse All Accounts    \n"));
	strcpy( menu_1[4].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_43, "3  Modify Account Password\n"));
	strcpy( menu_1[5].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_44, "4  Remove Account         \n"));
	strcpy( menu_1[6].line, "");
	strcpy( menu_1[7].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_45, "Enter \'e\' to return to previous menu or exit."));
	strcpy( menu_1[8].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_46, "Type a number followed by a carriage return"));
	strcpy( menu_1[9].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_47, "to execute a selection: "));

	strcpy( menu_111[0].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_48, "        TAXI Administration"));
	strcpy( menu_111[1].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_49, "          ADD ACCOUNTS"));
	strcpy( menu_111[2].line, "");
	strcpy( menu_111[3].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_50,
					  "Enter the person's login name (7 or fewer characters): "));
	strcpy( menu_111[4].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_51,
					  "Enter this person's full name (22 or fewer characters): "));
	strcpy( menu_111[5].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_52,
					  "ACCOUNT TYPE     LOGIN NAME     ACCOUNT ID     FULL NAME"));
	strcpy( menu_111[6].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_53, "Is this information correct (Y/N)? N"));
	strcpy( menu_111[7].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_54, "Do you wish to enter another account (Y/N)? N"));

	errorstr[0] = m_catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_55, "Type <return> to continue.");
    errorstr[1] = m_catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_56, "Reply NOT in range of choices.");

	strcpy( menu_12[0].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_39, "        TAXI Administration"));
	strcpy( menu_12[1].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_57, "          BROWSE ACCOUNTS"));
	strcpy( menu_12[2].line, "");
	strcpy( menu_12[3].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_58, "LOGIN NAME     USER ID     FULL NAME"));
	strcpy( menu_12[4].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_53, "Is this information correct (Y/N)? N"));
	strcpy( menu_12[5].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_59, "Do you wish to enter more information (Y/N)? N"));

	strcpy( menu_11[0].line,  catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_39, "  TAXI Administration"));
	strcpy( menu_11[1].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_66, "   ADD ACCOUNTS MENU"));
	strcpy( menu_11[2].line, "");
	strcpy( menu_11[3].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_60, "1  TAXI Call_Taker      \n"));
	strcpy( menu_11[4].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_61, "2  TAXI Dispatcher      \n"));
	strcpy( menu_11[5].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_62, "3  TAXI Manager         \n"));
	strcpy( menu_11[6].line, "");
	strcpy( menu_11[7].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_45, "Enter \'e\' to return to previous menu or exit."));
	strcpy( menu_11[8].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_46, "Type a number followed by a carriage return"));
	strcpy( menu_11[9].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_47, "to execute a selection: "));

	strcpy( menu_113[0].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_39, "        TAXI Administration"));
	strcpy( menu_113[1].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_63, "          CHANGE PASSWD"));
	strcpy( menu_113[2].line, "");
	strcpy( menu_113[3].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_64, "Enter the person's login name or account id: "));
	strcpy( menu_113[4].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_52,
					  "ACCOUNT TYPE     LOGIN NAME     ACCOUNT ID     FULL NAME"));
	strcpy( menu_113[5].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_53, "Is this information correct (Y/N)? N"));

	strcpy( menu_14[0].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_39, "        TAXI Administration"));
	strcpy( menu_14[1].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_65, "          REMOVE ACCOUNT"));
	strcpy( menu_14[2].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_58, "LOGIN NAME     USER ID     FULL NAME"));
	strcpy( menu_14[3].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_53, "Is this information correct (Y/N)? N"));
	strcpy( menu_14[4].line, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_59, "Do you wish to enter more information (Y/N)? N"));
}

/*====================================================================
= admin_accounts() -
=	Display Account Maintenance options.
=	Runs as a main program.
=====================================================================*/
main()
{
	register int indx;	/* index for loops  */
	int arraylen=arraysize1-1;
        char nlspath[80];
        static char put_nlspath[80];
        int  putenv();	
	char reply[60];		/* reply buffer */

	/*
	 * Set up for internationalization by setting the locale and opening the catalog
	 */

	umask(0);   /* give full rights all file created in here */
	strcpy(nlspath, "/usr/taxi/catalogs/finnish/%N.cat");
	sprintf(put_nlspath, "NLSPATH=%s", nlspath);
	putenv(put_nlspath);			
	setlocale(LC_ALL, "");
        init_mgr_locale();	
	open_catalog(&COMMON_m_catd, "common", 0, 0);
	open_catalog(&AMGMT_m_catd, "adminmgmt", 0, 0);

	menus_init();
	/* initialize account management with file locks */
	admin_init();

	while (1) { /* loop until a deliberate exit */

		/* clear the screen */
		fflush(stdout);
		clear_scrn();

		/* draw the main menu */
		putchar('\n');	
		for (indx=0; indx<arraylen; indx++)
			printf("%s%s\n",blanks,menu_1[indx].line);

		/* Prompt for a choice */
		printf("%s%s",blanks,menu_1[arraylen].line);

		fflush(stdin);
		gets(reply); /* get reply */
		switch (toupper(reply[0])) {
		case M_EXIT: /* return from this menu  */
			admin_cleanup();
			catclose( COMMON_m_catd );
			catclose( AMGMT_m_catd );
			return(0);
			break;
		 case '1': /* add a user to the TAXI system */
			admin_menu11();
			break;
		case '2': 
			admin_browse();
			break;
		case '3': 
			admin_modify();
			break;
		case '4': 
			admin_remove();
			break;
		default:
			printf("%s%s\n",blanks,errorstr[1]);
			printf("%s%s",blanks,errorstr[0]);
			fflush(stdin);
			fflush(stdout);
			getret();
			break;
		} /* end switch */
	} /* end while 1 */


}  /* end main */

/*====================================================================
= admin_init() -
=	Initialize the administration accounts utilities
=	Makes file and program locks by copying /etc/passwd and /etc/group
=====================================================================*/
admin_init()
{
	int capture;	/* scratch variable */

	FILE *fp;

	char systembuff[132];
	char iobuff[132];
	char reply;		/* single reply character */

	/* inhibit interrupts, call admin_clean from any interrupt */
	signal(SIGILL,admin_cleanup);	/* illegal instruction */
	signal(SIGTRAP,admin_cleanup);	/* caught system TRAP instruction */
	signal(SIGHUP, admin_cleanup);	/* hangup */
	signal(SIGINT, admin_cleanup);	/* interrupt */
	signal(SIGQUIT, admin_cleanup);	/* quit */
	signal(SIGTERM, admin_cleanup);	/* hangup */
	signal(SIGIOT, admin_cleanup);	/* an IOT instruction */
	signal(SIGABRT, admin_cleanup);	/* an aborted instruction */
	signal(SIGEMT,admin_cleanup);	/* EMT instruction */
	signal(SIGFPE,admin_cleanup);	/* floating point exception */
	signal(SIGBUS,admin_cleanup);	/* bus error */
	signal(SIGSEGV,admin_cleanup);	/* segmentation violation */
	signal(SIGSYS,admin_cleanup);	/* bad argument to system call */

	/* Be certain there is a passwd file */
	if ( access("/etc/passwd",FEXIST)) {
		/* NO /etc/passwd file */
		printf("%s%s.\n",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_1, "Cannot find /etc/passwd file") );
		printf("%s%s.\n",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_2,
						 "Please repair /etc/passwd and run this program again") );
		printf("%s%s", blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_3, "<cr> to continue..") );
		fflush(stdin);
		fflush(stdout);
		gets(iobuff);
		exit(-1);
	}

	/* Be certain there is a group file */
	if ( access("/etc/group",FEXIST)) {
		/* NO /etc/group file */
		printf("%s%s.\n",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_4, "Cannot find /etc/group file") );
		printf("%s%s.\n",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_5,
						 "Please repair /etc/group and run this program again") );
		printf("%s%s", blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_3, "<cr> to continue..") );
		fflush(stdin);
		fflush(stdout);
		gets(iobuff);
		exit(-2);
	} /* end of access */

	/* set the real userid to root */
	setuid(0); /* 0 = root */
}

/*====================================================================
= admin_menu11() -
=	Choices for adding users to system.
=	Choose CALL_TAKER, DISPATCHER, or MANAGER 
=====================================================================*/
admin_menu11()
{
	register int indx;	/* index for loops  */
	int arraylen=arraysize11-1;
	int n;	/* scratch variable */

	char reply[80];		/* single reply character */

	while (1) {

		/* clear the screen */
		fflush(stdout);	
		clear_scrn();

		/* draw the main menu, pick a type of account to add */
		putchar('\n');	
		for (indx=0; indx<arraylen;indx++) 
			printf("%s%s\n",blanks,menu_11[indx].line);

		/* prompt without a carriage return */
		printf("%s%s",blanks,menu_11[arraylen].line);

		fflush(stdin);
		gets(reply); /* get reply */
		switch (toupper(reply[0])) {
		case M_EXIT: /* return from this menu  */
			return;
		case '1': /* a valid menu option */
		case '2': 
		case '3': 
			n=reply[0]-'1';
#ifdef DEBUG
	sprintf(trace_str,"menu11: reply is %s call admin_add(%d)\n",reply,n);
	TRACE(trace_str);
#endif
			admin_add(n);
			break;
		default:
			printf("\n%s%s\n",blanks,errorstr[1]);
			printf("%s%s",blanks,errorstr[0]);
			fflush(stdout);
			fflush(stdin);
			getret();
			break;
		} /* end switch */
	} /* end while 1 */
}  /* end admin_menu11 */

/*====================================================================
= admin_add() -
=	Add call_handlers, dispatchers, and/or managers to UNIX.
=	Runs the user profile program up_maint() defining a user with default attributes.
=====================================================================*/
admin_add(usertype)
int usertype;
{
	int indx;
	int usernum;	
	int maxuserid;	
	int linex;	
	int field_length;
	int ok_to_add;
	char x;
    static char passpath[] = PASSWD_SAVE;
    static char grouppath[] = GROUP_SAVE;
    int i;

	char systembuff[132];
	char iobuff[300];
	char firstfield[32]; 	/* the login name */
	char *userdir; 		/* user id string */
	char *userid; 		/* user id string */
	char person[32]; 		/* person name */
	char answer;	/* single reply character */
	char *tmpptr;
	int  c;

	FILE  *fp;	/* file pointer for temp password file */

	/* Clean the stdin and stdout */
	fflush(stdin); 
	fflush(stdout);

	while (1) {
		/* clear the screen */
		clear_scrn();

		/* draw the main menu */
		putchar('\n');	
		for (indx=0; indx<options2; indx++) 
			printf("%s%s\n",blanks,menu_111[indx].line);

		/* Prompt for a login name */
		printf("\n%s%s",blanks,menu_111[indx++].line);
		fflush(stdout);
		fflush(stdin);
		gets(firstfield); 
		field_length=strlen(firstfield);
		if (! field_length) /* return from <cr> */ return;
		/* the login name is restricted to 7 alphanumeric characters */
		ok_to_add=1;				/* 1 will proceed 0 will loop */
		if (field_length >7) ok_to_add=0;
		else while (field_length--) 
			if (! isalnum(firstfield[field_length])) {
				printf("%s%s.\n",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_6,
								 "Please enter alphanumeric characters") );
				printf("%s%s",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_7, "<cr> to continue...") );
				gets(firstfield);
				ok_to_add=0;
				field_length=0;
			}
		if (! ok_to_add) continue;		/* must loop */

		/* The login name is ok. Prompt for a full user name */
		printf("\n%s%s",blanks,menu_111[indx++].line);
		fflush(stdin);

		i = 0;
		while ( ( c = getc(stdin) ) != '\n' )
		  {
		    if ( isprint( c ) )
		      {
			person[i] = c;
			i++;
		      }
		  }

        field_length = strlen(person);
		if ( field_length > USERNAME_MAX ) 
        {
           person[USERNAME_MAX] = EOS;	/* truncate to save in isam field */
           field_length = USERNAME_MAX;
        }
#ifdef FOO
        i = 0;                  /* verify user name is alphanumeric */
        while ( i < field_length )
        {
            /* prohibit non-printable characters */
	    c = (unsigned int)person[i];
            if (!isprint(c))
            {
                printf("%s%s.\n", blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_6,
                       "Please enter alphanumeric characters"));
                printf("%s%s", blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_7,
                       "<cr> to continue..."));

                fflush(stdin);
                gets(person);

                clear_scrn();
                printf("\n\n\n\n\n\n\n%s%s",blanks,menu_111[4].line);

                fflush(stdin);
                gets(person);

                field_length = strlen(person);
                if ( field_length > USERNAME_MAX )
                {
                    person[USERNAME_MAX] = EOS;	
                    field_length = USERNAME_MAX;
                }
                i = 0;
            }
            else
                i++;
	  }
#endif
            
		/* Copy new password and group files to be extra carefull. */
		sprintf(systembuff,"umask 0; touch %s %s",PASSWD_SAVE,GROUP_SAVE); /* ensure full rights */
		system(systembuff);
		sprintf(systembuff,"/bin/cp /etc/passwd %s; /bin/cp /etc/group %s",PASSWD_SAVE,GROUP_SAVE);
		system(systembuff);
		/* pick up exactly the login name */
		sprintf(systembuff,"umask 0; /bin/grep '^%s:' %s > %s",firstfield,PASSWD_SAVE,SCRATCH_FILE);
#ifdef DEBUG
	sprintf(trace_str,"add: %s\n",systembuff);
	TRACE(trace_str);
#endif
		system(systembuff); 	/* make the scratch password file */
	
		/* Open the scratch file for read only */
		if (! (fp=fopen(SCRATCH_FILE,"r")) ) {
			/* cannot read the scratch file */
			printf("\n%s%s %s %s.\n",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_8, "Cannot read"), SCRATCH_FILE,
			       catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_9, "file") );
			fflush(stdout);
			fflush(stdin);
			gets(firstfield); 
			return(-1);
		} /* end else no scratch file */

		linex=0;
		while (fgets(iobuff,132,fp)!=NULL)  linex++; 	/* count the lines */
		fclose(fp); 	/* close the scratch file */
		unlink(SCRATCH_FILE);

		if (linex) { /* Was NOT a unique user name */
			printf("\n%s%s\n",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_10, "The user name is not unique") );
			printf("%s%s",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_3, "<cr> to continue..") );
			fflush(stdout);
			fflush(stdin);
			gets(firstfield); 
			continue;
		} /* end lines */

		/* Open the temporary passwd file */
		if (! (fp=fopen(PASSWD_SAVE,"r")) ) {
			/* cannot read the password file */
			printf("\n%s%s %s %s.\n",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_8, "Cannot read"),
			       PASSWD_SAVE, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_9, "file") );
			printf("%s%s",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_3, "<cr> to continue..") );
			fflush(stdout);
			fflush(stdin);
			gets(firstfield); 
			continue;
		} /* end else no temporary group file */

		maxuserid=0;
		while (fgets(iobuff,132,fp)!=NULL)  {	/* look for the largest userid */
			/* read and parse a password line */
			if((userid=admin_field(iobuff,COLON)) != NULL){	/* skip the login name */
				userid=admin_field(userid,COLON);	/* skip the password */
				(void) admin_field(userid,COLON);	/* isolate the userid */
				usernum=atoi(userid); /* translate the corresponding usertype */
				if (usernum > 60000)
				  continue;
				if ( usernum != 65534 && usernum != 60001 )
				  maxuserid = (usernum>maxuserid) ? usernum : maxuserid;	/* the larger userid */
			}
		}	/* end while lines in file */
		fclose(fp);
		maxuserid++;

#ifdef DEBUG
	sprintf(trace_str,"add: maxuserid %d \n",maxuserid);
	TRACE(trace_str);
#endif

/* Echo check the information */
		printf("\n\n%s%s\n",blanks,menu_111[indx++].line);
		printf("\n%s%-17s%-15s%-15d%s\n",
		blanks,admin_account[usertype],firstfield,maxuserid,person);
		printf("\n%s%s%c",blanks,catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_11,
						 "Do you want to add this account? (Y/N) N"), '\010');
		fflush(stdin);
		if (toupper(getret()) != YES)
		    continue;

/* Build the passwd line */
		/* example: egor::412:400:Egor Shulman:/usr/taxi/egore:progname */
		/* Put call takers in a single directory */

		switch(usertype)
		  {
			case 0: userdir = CALLDIR;
				break;
			case 1: userdir = DISPDIR;
				break;
			case 2: userdir = MANDIR;
				break;
			default:
					userdir = CALLDIR;
				break;
		  }

		sprintf(iobuff,"%s::%d:%d:%s:%s%s:%s\n",
			firstfield,maxuserid,admin_group[usertype], person,HOMEDIR,userdir,SHELL);

#ifdef DEBUG
	sprintf(trace_str,"add: %s\n",iobuff);
	TRACE(trace_str);
#endif

		/* Open the temporary password file for append only */
		if ( fp=fopen(PASSWD_SAVE,"a") ) {
			/* add the person to the password file */
			fprintf(fp,iobuff);
			fclose(fp); 	/* close the temporary password file */
		} /* end open temporary password file */
		else { /* cannot open the tempfile for append */
			printf("%s %s.\n", catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_12, "Cannot find file"), PASSWD_SAVE);
			return(-11);
		} /* end else no temporary password file */

/* add the user to the temporary group file */
		/* first add the login name to the MADS group */
		/* get the group line */
		sprintf(systembuff,"umask 0; /bin/grep :%d: %s > %s", MADS_GROUP,GROUP_SAVE,SCRATCH_FILE);
#ifdef DEBUG
	sprintf(trace_str,"add: %s\n",systembuff);
	TRACE(trace_str);
#endif
		system(systembuff);
	
		/* Open the scratch file to read the group line */
		if ( (fp=fopen(SCRATCH_FILE,"r")) && (! fgets(iobuff,132,fp)==NULL))  {	
			/* add the new account name */
			fclose(fp); 	/* close the temporary password file */
			unlink(SCRATCH_FILE);
			indx=strlen(iobuff)-1;
			tmpptr=strrchr(iobuff,COLON)+1; /* point to the last colon */
			while (*tmpptr && (*tmpptr) == ' ') tmpptr++; /* scan for any character */
			/* replace the return with a comma or a null */
			if (strlen(tmpptr)>1) iobuff[indx]=COMMA;/* append the COMMA */
			else iobuff[indx]='\0';/* null the return */
			strcat(iobuff,firstfield);
		} /* end open SCRATCH_FILE */
		else { /* cannot open the tempfile for append */
			printf("\n%s%s %d %s %s.\n", blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_13, "Cannot find group"),
			       admin_group[usertype],catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_14, "in file"), GROUP_SAVE);
			printf("%s%s",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_3, "<cr> to continue..") );
			fflush(stdout); fflush(stdin); gets(firstfield); 
			continue;
		} /* end else no temporary group file */
	
		/* copy the group file without the new group line */
		sprintf(systembuff,"umask 0; /bin/grep -v :%d: %s > %s", MADS_GROUP,GROUP_SAVE,SCRATCH_FILE);
#ifdef DEBUG
	sprintf(trace_str,"add: %s\n",systembuff);
	TRACE(trace_str);
#endif
		system(systembuff);
	
		/* Open the scratch file to append the new group new line */
		if (! (fp=fopen(SCRATCH_FILE,"a")) ) {
			/* cannot read the scratch file */
			printf("\n%s%s %s %s.\n",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_8, "Cannot read"), SCRATCH_FILE,
			       catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_9, "file") );
			fflush(stdout); fflush(stdin); gets(firstfield); 
			return(-1);
		} /* end else no scratch file */
#ifdef DEBUG
	sprintf(trace_str,"add: write [%s]\n",iobuff);
	TRACE(trace_str);
#endif
		fprintf(fp,iobuff); putc('\n',fp);	/* add the new line with a return */
		fclose(fp);
		sprintf(systembuff,"/bin/mv %s %s",SCRATCH_FILE,GROUP_SAVE);
		system(systembuff);

/* second add the login name to the manager, dispatcher, or call handler group */
		/* get the group line */
		/* June 1988: This line made trouble for a new system */
		sprintf(systembuff,"umask 0; /bin/grep :%d: %s > %s",admin_group[usertype],GROUP_SAVE,SCRATCH_FILE);
		system(systembuff);
	
		/* Open the scratch file to read the group line */
		if ( (fp=fopen(SCRATCH_FILE,"r")) && 
			(! fgets(iobuff,132,fp)==NULL))  {	
			/* add the new account name */
			fclose(fp); 	/* close the temporary password file */
			unlink(SCRATCH_FILE);
			indx=strlen(iobuff)-1;
			tmpptr=strrchr(iobuff,COLON)+1; /* point to the last colon */
			while (*tmpptr && (*tmpptr) == ' ') tmpptr++; /* scan for any character */
			/* replace the return with a comma or a null */
			if (strlen(tmpptr)>1) iobuff[indx]=COMMA;/* append the COMMA */
			else iobuff[indx]='\0';/* null the return */
			strcat(iobuff,firstfield);
		} /* end open SCRATCH_FILE */
		else { /* cannot open the tempfile for append */
			printf("\n%s%s %d %s %s.\n",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_13, "Cannot find group"),
			       admin_group[usertype], catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_14, "in file"), GROUP_SAVE);
			printf("%s%s",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_3, "<cr> to continue..") );
			fflush(stdout); fflush(stdin); gets(firstfield); 
			continue;
		} /* end else no temporary group file */
	
		/* copy the group file */
		sprintf(systembuff,"umask 0; /bin/grep -v :%d: %s > %s",admin_group[usertype],GROUP_SAVE,SCRATCH_FILE);
		system(systembuff);
	
		/* Open the scratch file to append a new line */
		if (! (fp=fopen(SCRATCH_FILE,"a")) ) {
			/* cannot read the scratch file */
			printf("\n%s%s %s %s.\n",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_8, "Cannot read"), SCRATCH_FILE,
			       catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_9, "file") );
			fflush(stdout); fflush(stdin);
			gets(firstfield); 
			return(-1);
		} /* end else no scratch file */

#ifdef DEBUG
	sprintf(trace_str,"add: write [%s]\n",iobuff);
	TRACE(trace_str);
#endif
		fprintf(fp,iobuff);
		putc('\n',fp);
		fclose(fp);
		sprintf(systembuff,"/bin/mv %s %s",SCRATCH_FILE,GROUP_SAVE);
		system(systembuff);

	/* if temporary passwd and group files are not empty, copy to /etc */
		if( stat(passpath, &fstat_buf ) == 0 )
		  {
		    if ( fstat_buf.st_size > 0 )
		      {
			sprintf( systembuff, "/bin/cp %s /etc/passwd", PASSWD_SAVE );
			system(systembuff);
		      }
		    else
		      {
			ERROR(' ', "", "/etc/passwd not updated...temp file was empty");
			printf("%s%s.\n", blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_67,
							  "Warning: unable to update password file...request assistance"));
			printf("%s%s", blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_7,
						       "<cr> to continue..."));
			
			fflush(stdout);
			fflush(stdin);
			gets(iobuff);
			
			return(-1);
		      }
		  }
		else
		  {
		    ERROR(' ', "", "/etc/passwd not updated...temp file was empty");
		    printf("%s%s.\n", blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_67, 
						      "Warning: unable to update password file...request assistance"));
		    printf("%s%s", blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_7, 
						   "<cr> to continue..."));
		    
		    fflush(stdout);
		    fflush(stdin);
		    gets(iobuff);
		    
		    return(-1);
		  }
		


		if( stat( grouppath, &fstat_buf ) == 0 )
		  {
		    if ( fstat_buf.st_size > 0 )
		      {
			sprintf(systembuff,"/bin/cp %s /etc/group", GROUP_SAVE);
			system(systembuff);
		      }
		    else
		      {
			ERROR(' ', "", "/etc/group not updated...temp file was empty");
			printf("%s%s.\n", blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_68,
							  "Warning: unable to update group file...request assistance"));
			printf("%s%s", blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_7,
						       "<cr> to continue..."));
			
			fflush(stdout);
			fflush(stdin);
			gets(iobuff);
			
			return(-1);
		      }
		  }
		else
		  {
		    ERROR(' ', "", "/etc/group not updated...temp file was empty");
		    printf("%s%s.\n", blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_68, 
						      "Warning: unable to update group file...request assistance"));
		    printf("%s%s", blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_7, 
						   "<cr> to continue..."));
		    
		    fflush(stdout);
		    fflush(stdin);
		    gets(iobuff);
		    
		    return(-1);
		  }



/* Add a password for the new account */
		printf("\n%s%s, \n",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_15, "Enter a password of 6 to 8 characters") );
		printf(blanks);
		printf(catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_16, "Use at least 2 letters and 1 number (such as route1).\n"));
		fflush(stdout);
		sprintf(systembuff,"passwd %s", firstfield);
		system(systembuff); /* create the password */

		/* the user profile will have group-type default values */
		/* for 1=call handler, 2=dispatcher, 3=manager */
		/* this program runs without interaction */
		printf("%s%s.",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_17,
					       "One moment please: Adding the new person to the TAXI system") );
		fflush(stdout);
		sleep(2);
		/* calling up_maint will automatically synchornize the password and group files */
		sprintf(systembuff,"/usr/taxi/bin/up_maint %s %d",firstfield, usertype+up_differential);
		system(systembuff);	/* run the user profile program with the login name and the usertype */

		/* Prompt to enter another user of the same class */
		printf("\n%s%s %s %s\b",
		       blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_18,"Do you wish to enter another"), admin_account[usertype],
		       catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_19, "account? (Y/N) N") );
		fflush(stdout);
		fflush(stdin);
		if (toupper(getret()) != YES)
		    return(0);

	} /* end while 1 */
}  /* end admin_add() */

/*====================================================================
= admin_browse() -
=	Display the three type of users,
=	pausing for each page of output.
=====================================================================*/
admin_browse()
{
	register int indx;
	int  groupnum;
	int linex;
	int usertype;

	char systembuff[132];
	char *firstfield;
	char *userid;
	char *groupid;
	char *person;

	FILE  *fp;	/* file pointer for temp password file */

	/* Copy the password and group files */
	sprintf(systembuff,"umask 0; touch %s %s",PASSWD_SAVE,GROUP_SAVE); /* ensure full rights */
	system(systembuff);
	sprintf(systembuff,"cp /etc/passwd %s;cp /etc/group %s",PASSWD_SAVE,GROUP_SAVE);
	system(systembuff);

	/* scroll the user information , with limited paging */
	for (usertype=0; usertype<3; usertype++) {

		/* clear the screen */
		clear_scrn();

		/* draw the title */
		putchar('\n');	
		for (indx=0; indx<titlelen; indx++)
			printf("%s%s\n\n",blanks,menu_12[indx].line);

		linex=1;
		/* Open and rewind the temporary password file for read only */
		if (! (fp=fopen(PASSWD_SAVE,"r")) ) {
			/* cannot read the tempfile */
			printf("%s%s.\n",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_20,
							 "Cannot read temporary password file") );
			printf("%s%s",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_3, "<cr> to continue..") );
			fflush(stdin);
			fflush(stdout);
			gets(systembuff);
			exit(-1);
		} /* end else no temporary password file */

		printf("%s%s%s\n",blanks,"               ",admin_account[usertype]);
		printf("%s%s\n",blanks,menu_12[indx].line);
		linex += 4;

		while (fgets(systembuff,132,fp)!=NULL) {

			/* read and parse a password line */
			firstfield=systembuff;
			userid=admin_field(firstfield,COLON);
			userid=admin_field(userid,COLON); /* skip the password */
			groupid=admin_field(userid,COLON);
			person=admin_field(groupid,COLON);
			(void) admin_field(person,COLON);

			/* convert to numeric */
			groupnum=atoi(groupid);

			if (groupnum==admin_group[usertype]) {
				/* since there is more to display, */
				/* stop the scrolling with a full screen */
				if (linex>=pagelimit) {
					linex=1;
					printf("\n%s%s",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_21,
									"<cr> to continue....") );
					fflush(stdin);
					(void) getret();
					clear_scrn();
				}

				/* matches current group id */
				printf("\n%s",blanks);
				printf("%-17s%-10s%s\n",firstfield,userid,person);
				linex += 2;
			}
		}

		printf("\n%s%s",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_21, "<cr> to continue....") );
		fflush(stdin);
		(void) getret();
		clear_scrn();
		fclose(fp);
	}

	/* Remove the temp password and group files */
	sprintf(systembuff,"/bin/rm -f %s %s", PASSWD_SAVE, GROUP_SAVE);
	system(systembuff);
}

/*====================================================================
= admin_modify() -
=	Change a password in the temporary password file.
=	This procedure must be run as root.
=====================================================================*/
admin_modify()
{
int usertype=1;
	register int indx;
	int linex;	/* simple accumulator */
	int replylen;

	char systembuff[132];
	char iobuff[132];
	char reply[75];
	char firstfield[32]; 	/* the login name */
	char *userid; 		/* user id string */
	char *groupid;
	char *person; 		/* person name */
	char answer;	/* single reply character */

	FILE  *fp;	/* file pointer for temp password file */

	/* Clean the stdin and stdout */
	fflush(stdin); 
	fflush(stdout);

	/* clear the screen */
	clear_scrn();

	/* draw the title */
	putchar('\n');	
	for (indx=0; indx<titlelen; indx++) 
		printf("%s%s\n\n",blanks,menu_113[indx].line);
	printf("\n%s%s",blanks,menu_113[indx].line);

	gets(reply);
	if (! strlen(reply)) return;
	/* determine if the reply was a userid */
	replylen=strlen(reply);

#ifdef DEBUG
	sprintf(trace_str,"modify: [%s] length %d\n",reply,replylen);
	TRACE(trace_str);
#endif

	for (indx=0; indx<replylen; indx++)
		if (! isdigit(reply[indx]) ) break; /* must be a login name */

	/* Copy the password and group files */
	sprintf(systembuff,"umask 0; touch %s %s",PASSWD_SAVE,GROUP_SAVE);    /* ensure full rights */
	system(systembuff);
	sprintf(systembuff,"cp /etc/passwd %s; cp /etc/group %s",PASSWD_SAVE,GROUP_SAVE);
	system(systembuff);

	if (indx==replylen)	/* must be userid */
		sprintf(systembuff,"umask 0; /bin/grep %s %s > %s",reply,PASSWD_SAVE,SCRATCH_FILE);
	else			/* is the login name */
		/* pick up exactly the login name */
		sprintf(systembuff,"umask 0; /bin/grep '^%s:' %s > %s",reply,PASSWD_SAVE,SCRATCH_FILE);

#ifdef DEBUG
	sprintf(trace_str,"modify: %s\n",systembuff);
	TRACE(trace_str);
#endif

	system(systembuff); 	/* make the scratch password file */
	
	/* Open the scratch file for read only */
	if (! (fp=fopen(SCRATCH_FILE,"r")) ) {
		/* cannot read the scratch file */
		printf("\n%s%s %s %s.\n",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_8, "Cannot read"), SCRATCH_FILE,
		       catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_9, "file") );
		printf("%s%s",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_3, "<cr> to continue..") );
		fflush(stdin);
		fflush(stdout);
		gets(iobuff);
		exit(-1);
	} /* end else no scratch file */

	linex=0;
	while (fgets(iobuff,132,fp)!=NULL)  linex++; 	/* count the lines */
	fclose(fp); 	/* close the scratch file */

	if (linex == 0) {
			printf("\n%s%s %s.\n",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_22, "NO such ID"), reply);
			printf("%s%s",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_23, "<cr> to continue..") );
			fflush(stdin);
			fflush(stdout);
			gets(iobuff);
			return;
	} /* end line = 0 */

	if (linex > 1) {
			printf("\n%s%s %s %s.\n",blanks,catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_24, "ID"), reply,
			       catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_25, "is NOT unique") );
			printf("%s%s",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_3, "<cr> to continue..") );
			fflush(stdin);
			fflush(stdout);
			gets(iobuff);
			return;
	} /* end lines > 1 */

	/* lines = 1, read and parse a password line */
	userid=admin_field(iobuff,COLON);
	strcpy(firstfield,iobuff);
	userid=admin_field(userid,COLON); /* skip the password */
	groupid=admin_field(userid,COLON);
	person=admin_field(groupid,COLON);
	(void) admin_field(person,COLON);
	usertype=atoi(groupid); /* translate the corresponding usertype */

#ifdef DEBUG
	sprintf(trace_str,"modify: groupid [%s] usertype is %d\n",groupid,usertype);
	TRACE(trace_str);
#endif

	switch (usertype) {
	case MANGROUP: usertype=2; break;
	case DISPGROUP: usertype=1; break;
	case CALLGROUP: usertype=0; break;
	default:
		printf("\n%s%s %s\n",blanks,catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_26, "Invalid group number"), groupid);
		printf("%s%s\n",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_27, "Please repair /etc/passwd.") );
		printf("%s%s",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_3, "<cr> to continue..") );
		fflush(stdin);
		fflush(stdout);
		gets(iobuff);
		return;
	} /* end switch usertype */

#ifdef DEBUG
	sprintf(trace_str,"modify: usertype %d \n",usertype); 
	TRACE(trace_str);
#endif

	/* echo the user info to the administrator */
	printf("\n%s%s\n",blanks,menu_113[headerline].line);

	/* print the account information */
	printf("\n%s%-17s%-19s%-11s%s\n\n",
		blanks,admin_account[usertype],firstfield,userid,person);
	printf("%s%s%c",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_28, "Do you want to change this password? (Y/N) N"), '\010');
	fflush(stdin);
	answer=toupper(getret());
	if (answer != YES) {
		/* does NOT wish to capture system privledge */
		printf("\n%s%s.\n",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_29, "OK, Run this utility later") );
		printf("%s%s",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_3, "<cr> to continue..") );
		fflush(stdin);
		fflush(stdout);
		gets(iobuff);	
		return; /* return nicely */
	}

	/* Change the account password */
	printf("\n%s%s,\n",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_15, "Enter a password of 6 to 8 characters") );
	printf(blanks);
	printf(catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_16, "Use at least 2 letters and 1 number (such as route1).\n"));
        sprintf(systembuff, "passwd %s", firstfield );
#ifdef DEBUG
	sprintf(trace_str,"modify: %s\n",systembuff);
	TRACE(trace_str);
#endif
	system(systembuff); /* create the password */

	
}  /* end admin_modify() */

/*====================================================================
= admin_remove() -
=	Remove a login account 
=	The algorithm follows: prompt for a login name
=	or the userid.  Both should be unique entries
=	in the file /etc/passwd.
=	Grep the temporary file  
=	for the login or userid, thus verrifying:
=	1. the user exists and 2. the pattern is unique.
=	Echo the user information to the administrator.
=	Cancel the request or remove the user
=	from the password and group temporary files.
=	If the user has a directory, then remove the directory.
=====================================================================*/
admin_remove()
{
	register int indx;
	int replylen;
	int namelen;
	int usertype;	/* 0=calltaker, 1=dispatcher, 2=manager */
	int linex;	/* simple accumulator */
	long current_time;

	char systembuff[132];
	char iobuff[132];
	char reply[75];
	char firstfield[32]; 	/* the login name */
	char *userid; 		/* user id string */
	char *groupid;
	char *person; 		/* person name */
	char *name;
	char save_date[32];
	char answer;	/* single reply character */

	struct passwd *getpwnam(), *pw_ptr;	/* hold the passwd information */
	FILE  *fp;	/* file pointer for temp password file */

	/* clear the screen */
	clear_scrn();

	/* draw the title */
	putchar('\n');	
	for (indx=0; indx<2; indx++) {
		printf("%s",blanks);
		printf("%s\n\n",menu_14[indx].line);
	} /* end of title */

	fflush(stdin); fflush(stdout);
	printf("\n\n%s%s: ",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_30, "Enter the login name or account number") );
	gets(reply);
	/* determine if the reply was a userid */
	if (! (replylen=strlen(reply)) ) return;

	for (indx=0; indx<replylen; indx++)
		if (! isdigit(reply[indx]) ) break; /* must be a login name */

	if(indx==replylen)	/* must be userid */
		sprintf(systembuff,"umask 0; /bin/grep %s /etc/passwd > %s",reply, SCRATCH_FILE);
	else			/* is the login name */
		/* pick up exactly the login name */
		sprintf(systembuff,"umask 0; /bin/grep '^%s:' /etc/passwd > %s",reply, SCRATCH_FILE);

	system(systembuff); 	/* make the scratch password file */
	
	/* Open the scratch file for read only */
	if (! (fp=fopen(SCRATCH_FILE,"r")) ) {
		/* cannot read the scratch file */
		printf("\n%s%s %s %s.\n",blanks,catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_8, "Cannot read"),
		       SCRATCH_FILE, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_9, "file") );
		printf("\n%s%s",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_3, "<cr> to continue..") );
		fflush(stdin);
		fflush(stdout);
		gets(iobuff);
		exit(-1);
	} /* end else no scratch file */

	linex=0;
	while (fgets(iobuff,132,fp)!=NULL)  linex++; 	/* count the lines */
	fclose(fp); 	/* close the scratch file */

	if (linex == 0) {
		printf("\n%s%s %s.\n",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_22, "NO such ID"), reply);
		printf("%s%s",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_3, "<cr> to continue..") );
		fflush(stdin);
		fflush(stdout);
		gets(iobuff);
		return;
	}

	if (linex > 1) {
		printf("\n%s%s %s %s.\n",blanks,catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_24, "ID"), reply,
		       catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_25, "is NOT unique") );
		printf("%s%s",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_3, "<cr> to continue..") );
		fflush(stdin);
		fflush(stdout);
		gets(iobuff);
		return;
	}

	/* lines=1 is unique id, read and parse a password line */
	userid=admin_field(iobuff,COLON);
	strcpy(firstfield,iobuff);
	userid=admin_field(userid,COLON); /* skip the password */
	groupid=admin_field(userid,COLON);
	person=admin_field(groupid,COLON);
	(void) admin_field(person,COLON);
	usertype=atoi(groupid); /* translate the corresponding usertype */

	switch (usertype) {
	case MANGROUP: usertype=2; break;
	case DISPGROUP: usertype=1; break;
	case CALLGROUP: usertype=0; break;
	default:
		printf("\n%s%s %s\n",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_26, "Invalid group number"), groupid);
		printf("%s%s\n",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_27, "Please repair /etc/passwd."));
		printf("%s%s",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_3, "<cr> to continue..") );
		fflush(stdin);
		fflush(stdout);
		gets(iobuff);
		return;
	}

	/* echo the user info to the administrator */
	printf("\n\n%s               %s\n",blanks,admin_account[usertype]);
	printf("%s%s\n",blanks,menu_14[2].line);

	/* print the account information */
	printf("\n%s%-17s%-10s%s\n\n",blanks,firstfield,userid,person);

	printf("%s%s%c",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_31, "Do you want to remove this account? (Y/N) N"), '\010');
	fflush(stdin);
	if (toupper(getret()) != YES) {
		/* does NOT wish to capture system privledge */
		printf("\n%s%s.\n",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_29, "OK, Run this utility later") );
		printf("%s%s",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_3, "<cr> to continue..") );
		fflush(stdin);
		fflush(stdout);
		gets(iobuff);
		return;
	}

/* remove the login record from the up_maintenance table */
	printf("\n%s%s.",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_32,
					 "One moment please: Removing the person from the TAXI system") );
	fflush(stdout);
	sleep(2);
	sprintf(systembuff,"/usr/taxi/bin/up_maint %s %d",firstfield, up_removal);
	system(systembuff);	/* run the user profile program with the login name and the usertype */

}

/*====================================================================
= admin_cleanup() -
=	Repair any program specific features
=====================================================================*/
admin_cleanup(sigtype)
int sigtype;
{
	/* echo the interrupt type, if any interrupt ocurred */
	if (SIGHUP<=sigtype && sigtype<=SIGTERM)  {
		printf("\n%s%s %d %s.....\n",
		       blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_33, "Interrupt type"), sigtype,
		       catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_34, "trapped") );
		/* print a message and pause */
		printf("%s%s.....",blanks, catgets(AMGMT_m_catd, AMGMT_SET, AMGMT_35, "Cleaning up control files") );
		sleep(2);
		printf("\n\n");
	} /* end if interrupt */

	/* Be certain there is a temporary passwd file */
	if (! access(PASSWD_SAVE,FEXIST)) {
		/* remove temporary password file */
		unlink(PASSWD_SAVE);
	} /* end of access */

	/* Be certain there is a temporary group file */
	if (! access(GROUP_SAVE,FEXIST)) {
		/* remove temporary password file */
		unlink(GROUP_SAVE);
	} /* end of access */

	/* Remove any scratch file */
	if (! access(SCRATCH_FILE,FEXIST)) {
		/* remove temporary scratchfil */
		unlink(SCRATCH_FILE);
	} /* end of access */

	/* end the program */
	clear_scrn();
	exit(0);

} /* end of admin_cleanup */

/*====================================================================
= admin_field() -
=	Return a pointer to the next field in the buffer
=====================================================================*/
char *admin_field(stringbuff,match)
char *stringbuff;
char match;
{
	char *p1;

	/* read and parse a password line */
	/* Carefull: there may be no password */
	if((p1=strchr(stringbuff,match)) != NULL){
		*p1=NULL; /* isolates the login name */
		p1++;
	}
	return(p1);

}  /* end admin_field	*/