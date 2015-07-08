static char sz__FILE__[]="@(#)lang.c	10.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  lang.c; Rel: 5.9.0.0; Get date: 9/9/92 at 14:05:50
* @(#)  Last delta: 6/6/90 at 16:06:17
* @(#)  SCCS File: /taxi/sccs/s.lang.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/
/****************************************************************************
*
*       history:  date       author        revision
*                 04/18/90   r.barnett     internationalization
*                 05/18/90   r.barnett     added init_mgr_locale()
*
****************************************************************************
*
*       this file contains the following routines for language localization
*
*       init_catalog()         open language catalogue
*       cleanup_catalog()      close language catalogue
*       init_locale()          bind language environment variables
*       init_mgr_locale()      bind manager's language environment variables
*
***************************************************************************/

#include <stdio.h>
#include <nl_types.h>
#include <locale.h>
#include <strings.h>
#include "path.h"
#include "language.h"

#define NOT_IN_USE 0

/***************************************************************************
*
*    init_catalog()   open language catalogue
*
*    inputs:  catalogue name
*
*    outputs: catalogue descriptor
*
*    usage:   catd = init_catalog(catname)
*             returns catd if successful
*             returns -l if failed
*
*    written: r.barnett  4/18/90
*
***************************************************************************/
nl_catd init_catalog(catname)

char *catname;
{

   char buf[120];
   nl_catd catd;
   nl_catd catopen();
   int  oflag = NOT_IN_USE;
   char *getenv();
   char CatPathName[64];

   sprintf(CatPathName, "%s%s.cat", TPAK_NLS_PATH, catname);
   catd = catopen( CatPathName, oflag );

   if ( (int)catd <= 0)
     {
            return((nl_catd)-1);
    }

   return(catd);
}

/***************************************************************************
*
*    cleanup_catalog()   closes language catalogue
*
*    inputs:  catd, catalogue descriptor
*
*    outputs: none
*
*    usage:   cleanup_catalog(catd)
*             returns 0 if catalogue closed
*             returns -1 if unable to close catalogue
*
*    written: r.barnett  3/22/90
*
***************************************************************************/
int cleanup_catalog(catd)

nl_catd catd;
{

   char buf[80];
   char *getenv();
   char *lang;
   char *nlspath;
   int  catclose();
                                            /* close language catalogue */
   if ( catclose(catd) ) {
#ifdef DEBUG
      printf("\ncleanup_catalog: cannot close language catalogue\n");
      lang = getenv("LANG");
      nlspath = getenv("NLSPATH");
      sprintf(buf, "echo \"LANG=%s and NLSPATH=%s\n\"", lang, nlspath);
      system(buf);
#endif
      return(-1);
   }

   return(0);
}

/***************************************************************************
*
*    init_locale()    bind localization environment variables (for use
*                     with Ultrix/POSIX international language support).
*
*    inputs:  none
*
*    outputs: none
*
*    usage:   init_locale()
*             no returned values
*
*
*    written: r.barnett  4/18/90
*
***************************************************************************/
void init_locale()

{
  char sLocale[32];

  strcpy( sLocale, TPAK_LOCALE );
   if ( (setlocale (LC_ALL, sLocale)) == NULL ) {
#ifdef DEBUG
      printf("\ninit_locale: can't set locale...check $LANG $INTLINFO\n");
#endif
      ;
   }
   return;
}

/***************************************************************************
*
*    init_mgr_locale()    bind localization environment variables (for use
*                         with Ultrix/POSIX international language support),
*                         using the variables set in the manager's profile.
*
*    algorithm:
*        begin
*           open temp file
*           store system manager's $LANG variable in temp file
*           set $LANG environment variable
*           store system manager's $NLSPATH variable in temp file
*           set $NLSPATH environment variable
*           remove temp file
*           setlocale() to announce language environment
*        end
*
*    inputs:  none
*
*    outputs: none
*
*    usage:   init_mgr_locale()
*             no returned values
*
*
*    written: r.barnett  5/18/90
*
***************************************************************************/
void init_mgr_locale()

{
char nlspath[80];
int  putenv();
char put_nlspath[80];
char sLocale[80];

	sprintf(put_nlspath, "NLSPATH=%s", TPAK_NLS);
	putenv(put_nlspath);

        strcpy( sLocale, TPAK_LOCALE );
	/* setlocale() to announce language environment */
	if ( (setlocale (LC_ALL, sLocale)) == (char *)NULL ) {
		printf("\ninit_mgr_locale: can't set locale...check $LANG $INTLINFO\n");
		;
	}
}

Eightto7bitConvert( char *pChartoConvert )
{
  if ( ( !strcmp( LANGUAGE, "Svenska" ) ) ||
       ( !strcmp( LANGUAGE, "Finnish" ) ) )
    {
      switch( *pChartoConvert )
	{
	case 'Å':
	  *pChartoConvert = ']';
	  break;
	case 'Ä':
	  *pChartoConvert = '[';
	  break;
	case 'Ö':
	  *pChartoConvert = '\\';
	  break;
	default:
	  break;
	}
    }
  else if ( !strcmp( LANGUAGE, "Danish" ) )
    {
      switch( *pChartoConvert )
	{
	case 'Å' :
	  *pChartoConvert = ']';
	  break;
	case 'Æ' :
	  *pChartoConvert = '[';
	  break;
	case 'Ø':
	  *pChartoConvert = '\\';
	  break;
	default:
	  break;
	}
    }
}  

Sevento8bitConvert( char *pChartoConvert )
{
  if ( ( !strcmp( LANGUAGE, "Svenska" ) ) ||
       ( !strcmp( LANGUAGE, "Finnish" ) ) )
    {
      switch( *pChartoConvert )
	{
	case ']' :
	case '}' :
	  *pChartoConvert = 'Å';
	  break;
	case '[' :
	case '{' :
	  *pChartoConvert = 'Ä';
	  break;
	case '\\':
	case '|' :
	  *pChartoConvert = 'Ö';
	  break;
	case '^' :
	case '~' :
	  *pChartoConvert = 'Ü';
	  break;
	default:
	  break;
	}
    }
  else if ( !strcmp( LANGUAGE, "Danish" ) )

    {
      switch( *pChartoConvert )
	{
	case ']' :
	case '}' :
	  *pChartoConvert = 'Å';
	  break;
	case '[' :
	case '{' :
	  *pChartoConvert = 'Æ';
	  break;
	case '\\':
	case '|' :
	  *pChartoConvert = 'Ø';
	  break;
	default:
	  break;
	}
    }
}
