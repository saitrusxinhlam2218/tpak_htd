/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: ui_geo_mod.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 2001/11/21 19:21:19 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/ui/ui_geo_mod.c,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/

static char rcsid[] = "$Id: ui_geo_mod.c,v 1.2 2001/11/21 19:21:19 taxiadm Exp $";

#include <stdio.h>
#include "parsecfg.h"		/* include this header file when use parsecfg library */
#include "mads_types.h"
#include "mads_isam.h"
#include "language.h"
#include "key.h"
#include "mad_error.h"
#include "path.h"
#include "GeoAddr_Addr_db.h"

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

int *ini_bool;
int *ini_int;
unsigned int *ini_uint;
long *ini_long;
unsigned long *ini_ulong;
float *ini_float;
double *ini_double;
char **ini_str;
cfgList **ini_list;

cfgStruct cfgini[] = {
	/* parameter  type  address of variable */
  {"BOOL", CFG_BOOL, &ini_bool},
	{"LIST",   CFG_STRING_LIST, &ini_list},
	{NULL, CFG_END, NULL}	/* no more parameters */
};

geo_mod_init()
{
  cfgList *l;
  int max, i;
  int a;
  float f;
  char *b;
  cfgList *c = NULL;	/* cfgList for fetchVarFromCfgFile() must be initialized as NULL */

  max = cfgParse("/usr/taxi/run/geo_mod.ini", cfgini, CFG_INI);
  if (max == -1) {
    printf("an error was detected\n");
    return 1;
  }

}
