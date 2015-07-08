/**************************************************************************/
/*                                                                        */
/*  parsecfg - a library for parsing a configuration file                 */
/*  Copyright (C) 1999-2001 Yuuki NINOMIYA <gm@debian.or.jp>              */
/*                                                                        */
/*  This program is free software; you can redistribute it and/or modify  */
/*  it under the terms of the GNU General Public License as published by  */
/*  the Free Software Foundation; either version 2, or (at your option)   */
/*  any later version.                                                    */
/*                                                                        */
/*  This program is distributed in the hope that it will be useful,       */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/*  GNU General Public License for more details.                          */
/*                                                                        */
/*  You should have received a copy of the GNU General Public License     */
/*  along with this program; if not, write to the                         */
/*  Free Software Foundation, Inc., 59 Temple Place - Suite 330,          */
/*  Boston, MA 02111-1307, USA.                                           */
/*                                                                        */
/**************************************************************************/

/* $Id: geo_mod.c,v 1.1 2002/03/25 17:17:29 jwelch Exp $ */

#include <stdio.h>
#include "parsecfg.h"		/* include this header file when use parsecfg library */
#include "mads_types.h"
#include "mads_isam.h"
#include "language.h"
#include "key.h"
#include "mad_error.h"
#include "path.h"
#include "GeoAddr_Addr_db.h"


/************************/
/*    for CFG_SIMPLE    */
/************************/

/* variables stored got value */

/* If there is no parameter in the configuration file,
   the value is not changed.
   However if linked list isn't set initial value,
   it must be initialized as NULL. */

char tmp_req_buf[5120];
int cfg_bool;
int cfg_int;
int cfg_int_not_change = 489;
unsigned int cfg_uint;
float cfg_float;
double cfg_double;
long cfg_long;
unsigned long cfg_ulong;
char *cfg_str;			/* pointer to string (Memories stored real string is allocated dynamically.) */
cfgList *cfg_list = NULL;	/* pointer to linked list */
BOOLEAN geo_process=FALSE;
BOOLEAN geo_list = FALSE;
BOOLEAN geo_validate = FALSE;


/* array of cfgStruct */

cfgStruct cfg[] = {		/* this must be initialized */
	/* parameter  type       address of variable */
	{"BOOL",     CFG_BOOL,        &cfg_bool},
	{"INT",      CFG_INT,         &cfg_int},
	{"UINT",     CFG_UINT,        &cfg_uint},
	{"LONG",     CFG_LONG,        &cfg_long},
	{"ULONG",    CFG_ULONG,       &cfg_ulong},
	{"FLOAT",    CFG_FLOAT,       &cfg_float},
	{"DOUBLE",   CFG_DOUBLE,      &cfg_double},
	{"STR",      CFG_STRING,      &cfg_str},
	{"LIST",     CFG_STRING_LIST, &cfg_list},
	{"NoChange", CFG_INT,         &cfg_int_not_change},
	{NULL, CFG_END, NULL}	/* no more parameters */
};



/************************/
/*     for CFG_INI      */
/************************/

/* variables stored got value */

/* If there is no parameter in the configuration file,
   the value is initialized 0 or NULL (boolean value is
   initialized -1(FALSE)). */

int *ini_bool;
int *ini_int;
unsigned int *ini_uint;
long *ini_long;
unsigned long *ini_ulong;
float *ini_float;
double *ini_double;
char **ini_str;
cfgList **ini_list;


/* array of cfgStruct */

cfgStruct cfgini[] = {
	/* parameter  type  address of variable */
  {"BOOL", CFG_BOOL, &ini_bool},
	{"LIST",   CFG_STRING_LIST, &ini_list},
	{NULL, CFG_END, NULL}	/* no more parameters */
};


main(int argc, char **argv)
{
	cfgList *l;
	int max, i;
	int a;
	float f;
	char *b;
	cfgList *c = NULL;	/* cfgList for fetchVarFromCfgFile() must be initialized as NULL */
        GEOADDR_ADDR_REC geo_addr;
        int mode = 0;
        BOOLEAN done = FALSE;
        char    from[GEOADDR_SNAME_LEN + 1], to[GEOADDR_SNAME_LEN + 1], change[GEOADDR_SNAME_LEN + 1];
        char    sav_sname[GEOADDR_SNAME_LEN + 1];
        char    test_sname[GEOADDR_SNAME_LEN + 1];
        struct cisam_ad  addr;
        int count = 0;
        BOOLEAN db_change = FALSE;


        parse_cmd(argc, argv);
        
	max = cfgParse("/usr/taxi/run/geo_mod.ini", cfgini, CFG_INI);
	if (max == -1) {
		printf("an error was detected\n");
		return 1;
	}

        if ( geo_list == TRUE )
          {
            for (i = 0; i < max; i++)
              {
                printf("\n[%s]\n", cfgSectionNumberToName(i));
                
                printf("BOOL  : %s\n", ini_bool[i] ? "TRUE" : "FALSE");	/* 1 is "TRUE", 0 is "FALSE" */
                
                
                if (ini_list[i] == NULL)
                  {
                    continue;
                  }
                for (l = ini_list[i]; l != NULL; l = l->next)
                  {
                    printf("LIST  : %s\n", l->str);
                  }
              }
          }


	c = NULL;




        
        if ( geo_process == TRUE )
          {
        
            init_mgr_locale();        
            
            db_open(GEOADDR_ADDR_FILE_ID, DB_READ_WRITE|DB_MANUAL_LOCK);
            mode = ISFIRST;
            done = FALSE;
            bzero( &geo_addr, sizeof(GEOADDR_ADDR_REC ) );
            sav_sname[0] = '\0';
            while ( !done )
              {
                
                if ( db_change == TRUE )
                  {
                    mode = ISGTEQ;
                    strcpy( geo_addr.sname, sav_sname );
                  }
                
                if ( db_read_key( GEOADDR_ADDR_FILE_ID, &geo_addr, &geoaddr_addr_key1, mode ) != SUCCESS )
                  break;
                mode = ISNEXT;
                
                db_change = FALSE;
                strcpy(sav_sname, geo_addr.sname);
                
                // Convert to UpperCase ?
                if (fetchVarFromCfgFile("/usr/taxi/run/geo_mod.ini", "BOOL", &a, CFG_BOOL, CFG_INI, cfgSectionNameToNumber("UpperCase")+1,NULL) == 0)
                  {
                    if (a) // true
                      {
                        UpperCase( geo_addr.sname );
                        // convert to upper
                        
                      }
                  }
                // Remove Spaces
                if (fetchVarFromCfgFile("/usr/taxi/run/geo_mod.ini", "BOOL", &a, CFG_BOOL, CFG_INI, cfgSectionNameToNumber("RemoveSpaces")+1,NULL) == 0)
                  {
                    if (a)
                      RemoveSpace( geo_addr.sname );
                  }
                
                // Convert special characters
                if (fetchVarFromCfgFile("/usr/taxi/run/geo_mod.ini", "BOOL", &a, CFG_BOOL, CFG_INI, cfgSectionNameToNumber("Characters")+1,NULL)==0)
                  {
                    if ( a )
                      {
                        count = 0;
                        for (l=ini_list[2]; l!= NULL; l = l->next)
                          {
                            if ( count % 2 )
                              strcpy(to, l->str);
                            else
                              strcpy(from, l->str);
                            
                            if ( count % 2 )
                              ModifyCharacters( geo_addr.sname, from, to );
                            ++count;
                          }
                      }
                  }
                
                // Convert beginning of strname
                if (fetchVarFromCfgFile("/usr/taxi/run/geo_mod.ini", "BOOL", &a, CFG_BOOL, CFG_INI, cfgSectionNameToNumber("Begins")+1,NULL)==0)
                  {
                    if ( a )
                      {
                        count = 0;
                        for (l=ini_list[3]; l!= NULL; l = l->next)
                          {
                            if ( count % 2 )
                              strcpy(to, l->str);
                            else
                              strcpy(from, l->str);
                            
                            if ( count % 2 )
                              {
                                if ( ModifyBegin( geo_addr.sname, from, to ))
                                  {
                                    db_update(GEOADDR_ADDR_FILE_ID, &geo_addr);
                                    db_change = TRUE;
                                  }
                              }
                            ++count;
                          }
                      }
                  }
                
                // Convert end of strname
                if (fetchVarFromCfgFile("/usr/taxi/run/geo_mod.ini", "BOOL", &a, CFG_BOOL, CFG_INI, cfgSectionNameToNumber("Ends")+1,NULL)==0)
                  {
                    if ( a )
                      {
                        count = 0;
                        for (l=ini_list[4]; l!= NULL; l = l->next)
                          {
                            if ( count % 2 )
                              strcpy(to, l->str);
                            else
                              strcpy(from, l->str);
                            
                            if ( count % 2 )
                              {
                                if ( ModifyEnd( geo_addr.sname, from, to ) )
                                  {
                                    db_update(GEOADDR_ADDR_FILE_ID, &geo_addr);
                                    db_change = TRUE;
                                  }
                              }
                            ++count;
                          }
                      }
                  }
                
                // Convert middle of strname
                if (fetchVarFromCfgFile("/usr/taxi/run/geo_mod.ini", "BOOL", &a, CFG_BOOL, CFG_INI, cfgSectionNameToNumber("Contains")+1,NULL)==0)
                  {
                    if ( a )
                      {
                        count = 0;
                        for (l=ini_list[5]; l!= NULL; l = l->next)
                          {
                            if ( count % 2 )
                              strcpy(to, l->str);
                            else
                              strcpy(from, l->str);
                            
                            if ( count % 2 )
                              {
                                if ( ModifyContains( geo_addr.sname, from, to ) )
                                  {
                                    db_update(GEOADDR_ADDR_FILE_ID, &geo_addr);
                                    db_change = TRUE;
                                  }
                              }
                            ++count;
                          }
                      }
                  }
                
#ifdef FOO
                if (fetchVarFromCfgFile("/usr/taxi/run/geo_mod.ini", "BOOL", &a, CFG_BOOL, CFG_INI, cfgSectionNameToNumber("Translations")+1,NULL)==0)
                  {
                    if ( a )
                      {
                        count = 0;
                        for (l=ini_list[6]; l!=NULL; l=l->next)
                          {
                            if ( count % 2 )
                              strcpy(to, l->str);
                            else
                              strcpy(from, l->str);
                            
                            if ( count % 2 )
                              ModifyTranslations( geo_addr.sname, from, to );
                            ++count;
                          }
                      }
                  }
#endif
                
                
              }
            
            db_close(GEOADDR_ADDR_FILE_ID);
          }

        if ( geo_validate == TRUE )
          {
            printf("Validating...\n");            
            init_mgr_locale();
            db_open(ADDRESS_FILE_ID, DB_READ_WRITE|DB_MANUAL_LOCK);
            db_open(GEOADDR_ADDR_FILE_ID, DB_READ_WRITE|DB_MANUAL_LOCK);
            mode = ISFIRST;
            done = FALSE;
            bzero( &addr, sizeof(struct cisam_ad) );
            while ( !done )
              {
                if ( db_read_key( ADDRESS_FILE_ID, &addr, &ad_key3, mode ) != SUCCESS )
                  break;
                strcpy( test_sname, addr.str_name );
                mode = ISNEXT;
                // don't process placenames
                if ( addr.low_nbr == 999 && addr.high_nbr == 999 )
                  continue;
                
                // process street name according to rules
                // Convert to UpperCase ?
                if (fetchVarFromCfgFile("/usr/taxi/run/geo_mod.ini", "BOOL", &a, CFG_BOOL, CFG_INI, cfgSectionNameToNumber("UpperCase")+1,NULL) == 0)
                  {
                    if (a) // true
                      {
                        UpperCase( addr.str_name );
                        // convert to upper
                        
                      }
                  }
                // Remove Spaces
                if (fetchVarFromCfgFile("/usr/taxi/run/geo_mod.ini", "BOOL", &a, CFG_BOOL, CFG_INI, cfgSectionNameToNumber("RemoveSpaces")+1,NULL) == 0)
                  {
                    if (a)
                      RemoveSpace( addr.str_name );
                  }
                
                // Convert special characters
                if (fetchVarFromCfgFile("/usr/taxi/run/geo_mod.ini", "BOOL", &a, CFG_BOOL, CFG_INI, cfgSectionNameToNumber("Characters")+1,NULL)==0)
                  {
                    if ( a )
                      {
                        count = 0;
                        for (l=ini_list[2]; l!= NULL; l = l->next)
                          {
                            if ( count % 2 )
                              strcpy(to, l->str);
                            else
                              strcpy(from, l->str);
                            
                            if ( count % 2 )
                              ModifyCharacters( addr.str_name, from, to );
                            ++count;
                          }
                      }
                  }
                
                // Convert beginning of strname
                if (fetchVarFromCfgFile("/usr/taxi/run/geo_mod.ini", "BOOL", &a, CFG_BOOL, CFG_INI, cfgSectionNameToNumber("Begins")+1,NULL)==0)
                  {
                    if ( a )
                      {
                        count = 0;
                        for (l=ini_list[3]; l!= NULL; l = l->next)
                          {
                            if ( count % 2 )
                              strcpy(to, l->str);
                            else
                              strcpy(from, l->str);
                            
                            if ( count % 2 )
                              {
                                if ( ModifyBegin( addr.str_name, from, to ))
                                  {
                                    //                                    db_update(GEOADDR_ADDR_FILE_ID, &geo_addr);
                                    //                                    db_change = TRUE;
                                  }
                              }
                            ++count;
                          }
                      }
                  }
                
                // Convert end of strname
                if (fetchVarFromCfgFile("/usr/taxi/run/geo_mod.ini", "BOOL", &a, CFG_BOOL, CFG_INI, cfgSectionNameToNumber("Ends")+1,NULL)==0)
                  {
                    if ( a )
                      {
                        count = 0;
                        for (l=ini_list[4]; l!= NULL; l = l->next)
                          {
                            if ( count % 2 )
                              strcpy(to, l->str);
                            else
                              strcpy(from, l->str);
                            
                            if ( count % 2 )
                              {
                                if ( ModifyEnd( addr.str_name, from, to ) )
                                  {
                                    //                                    db_update(GEOADDR_ADDR_FILE_ID, &geo_addr);
                                    //                                    db_change = TRUE;
                                  }
                              }
                            ++count;
                          }
                      }
                  }
                
                // Convert middle of strname
                if (fetchVarFromCfgFile("/usr/taxi/run/geo_mod.ini", "BOOL", &a, CFG_BOOL, CFG_INI, cfgSectionNameToNumber("Contains")+1,NULL)==0)
                  {
                    if ( a )
                      {
                        count = 0;
                        for (l=ini_list[5]; l!= NULL; l = l->next)
                          {
                            if ( count % 2 )
                              strcpy(to, l->str);
                            else
                              strcpy(from, l->str);
                            
                            if ( count % 2 )
                              {
                                if ( ModifyContains( addr.str_name, from, to ) )
                                  {
                                    //                                    db_update(GEOADDR_ADDR_FILE_ID, &geo_addr);
                                    //                                    db_change = TRUE;
                                  }
                              }
                            ++count;
                          }
                      }
                  }
                bzero(&geo_addr, sizeof(GEOADDR_ADDR_REC) );
                strcpy( geo_addr.sname, addr.str_name );
                strcpy( geo_addr.city, addr.str_city );
                if ( db_read_key( GEOADDR_ADDR_FILE_ID, &geo_addr, &geoaddr_addr_key1, ISEQUAL ) != SUCCESS )
                  printf( "%s %s --> %s\n", test_sname, addr.str_city, addr.str_name );                  
              }
            
          }

	return (0);
}

ModifyTranslations( char *s, char from[], char to[] )
{
  if ( !strcmp( s, from ) )
    sprintf( s, "%s", to );
}

ModifyContains( char *s, char from[], char to[] )
{
  int i;
  char Start[GEOADDR_SNAME_LEN+1], End[GEOADDR_SNAME_LEN+1];
  char *pS = NULL;
  
  pS = (char *)strstr( s, from );

  if ( pS != NULL )
    {
      strncpy( Start, s, (pS - s) );
      strncpy( End, (pS + strlen(from)), ((s+ strlen(s)) - (pS + strlen(from))) );
      sprintf( s, "%s%s%s", Start, to, End );
      return(1);
    }
  else
    return(0);
}

ModifyEnd( char *s, char from[], char to[] )
{
  char *pS=NULL;

  pS = (s + strlen(s) - strlen(from));
  if ( pS != NULL )
    {
      if ( !strncmp( from, pS, strlen(from) ) )
        {
          *pS = '\0';
          sprintf( s, "%s%s", s, to );
          return(TRUE);
        }
    }
  return(FALSE);
}

ModifyBegin( char *s, char from[], char to[] )
{
  int i;
  char *pS=NULL;

  if ( !strncmp( from, s, strlen(from) ) )
    {
      pS = (char *)strstr( s, from );
      if ( pS != NULL )
        {
          pS += strlen(from);
          strcpy(s,pS);
          sprintf( s, "%s%s", to, s );
          return(1);
        }
    }
  return(0);
}

ModifyCharacters( char s[], char from[], char to[] )
{
  int i;
  
  for ( i = 0; i < strlen(s); i++ )
    {
      if ( s[i] == from[0] )
        s[i] = to[0];
    }
}

UpperCase( char s[] )
{
  int i, a;

  for ( i = 0; i < strlen(s); i++ )
    {
      a = s[i];
      a = Toupper(a);
      s[i] = a;
    }
}

RemoveSpace( char s[] )
{
  int i;

  for ( i = strlen(s) - 1; i > 0; i-- )
    {
      if ( s[i] == ' ' )
        s[i] = '\0';
      else
        break;
    }
}


parse_cmd(argc,argv)
int argc;
char **argv;
{
	int c;
	extern char *optarg;

        while((c = getopt(argc,argv,"plv")) != -1)	
		switch(c){
                case 'p':
                  geo_process = TRUE;
                  break;
                case 'l':
                  geo_list = TRUE;
                  break;
                case 'v':
                  geo_validate = TRUE;
                  break;
		default:
                  usage();
                  break;
		}
        if ( geo_process && geo_validate )
          usage();
        
				
}

usage()
{
  printf("Usage:  geo_mod [ -v | -p | -l ]\n");
  printf("\t -v : validate TaxiPak geo_database against Tätort 2000\n");
  printf("\t -p : process Tätort 2000 tables according to rules in geo_mod.ini\n");
  printf("\t -l : list rules in geo_mod.ini\n");
  exit();
}
