/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: geo_mod.c,v $
*  @(#)  $Revision: 1.5 $
*  @(#)  $Date: 2003/01/28 15:56:28 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/common/geo_mod.c,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/

static char rcsid[] = "$Id: geo_mod.c,v 1.5 2003/01/28 15:56:28 jwelch Exp $";

#include <stdio.h>
#include <string.h>
#include "parsecfg.h"		/* include this header file when use parsecfg library */
#include "mads_types.h"
#include "mads_isam.h"
#include "language.h"
#include "key.h"
#include "mad_error.h"
#include "path.h"


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

BOOLEAN  cfg_init = FALSE;

/* array of cfgStruct */

cfgStruct cfgini[] = {
	/* parameter  type  address of variable */
  {"BOOL", CFG_BOOL, &ini_bool},
	{"LIST",   CFG_STRING_LIST, &ini_list},
	{NULL, CFG_END, NULL}	/* no more parameters */
};


geo_mod( char  sname[] )
{
	cfgList *l;
	int max, i;
	int a;
	float f;
	char *b;
	cfgList *c = NULL;	/* cfgList for fetchVarFromCfgFile() must be initialized as NULL */
        int count = 0;
        char    from[GEOADDR_SNAME_LEN + 1], to[GEOADDR_SNAME_LEN + 1];        
        char strname[21];

        if ( cfg_init == FALSE )
          {
            max = cfgParse("/usr/taxi/run/geo_mod.ini", cfgini, CFG_INI);
            if (max == -1) {
              return FAIL;
            }
            cfg_init = TRUE;
            
          }

        strcpy(strname, sname);

        // Convert to UpperCase ?
        if (fetchVarFromCfgFile("/usr/taxi/run/geo_mod.ini", "BOOL", &a, CFG_BOOL, CFG_INI, cfgSectionNameToNumber("UpperCase")+1,NULL) == 0)
          {
            if (a) // true
              {
                 UpperCase( strname );
                // convert to upper
                
              }
          }


            // Remove Spaces
        if (fetchVarFromCfgFile("/usr/taxi/run/geo_mod.ini", "BOOL", &a, CFG_BOOL, CFG_INI, cfgSectionNameToNumber("RemoveSpaces")+1,NULL) == 0)
          {
            if (a)
              RemoveSpace( strname );
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
                      ModifyCharacters( strname, from, to );
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
                        ModifyBegin( strname, from, to );
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
                        ModifyEnd( strname, from, to );
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
                        ModifyContains( strname, from, to );
                      }
                    ++count;
                  }
              }
          }


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
                      ModifyTranslations( strname, from, to );
                    ++count;
                  }
              }
          }


        strcpy( sname, strname );
	return (SUCCESS);
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



