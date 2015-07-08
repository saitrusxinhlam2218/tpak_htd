static char sz__FILE__[]="@(#)filesys.c	10.1.2.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  filesys.c; Rel: 10.1.2.1; Get date: 5/11/95 at 10:14:28
* @(#)  Last delta: 12/2/94 at 18:14:47
* @(#)  SCCS File: /taxi/sccs/s.filesys.c
*                                                                           *
* Copyright (c) 1991 - Motorola Seattle                                     *
*                                                                           *
*****************************************************************************/
#include <sys/types.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>

#include "mad_error.h"
#include "except.h"
#include "path.h"
#include "tmplen.h"

struct fs_info
{
  char name[81];
  int  threshold;
};


#define MAX_FILE_SYSTEMS	10
struct fs_info File_systems[MAX_FILE_SYSTEMS];
int  Num_file_systems = 0;

void
init_file_system_check()
{
  FILE *fp;
  int  i, rc;
  char line[81];
  char s_threshold[10];
	static char error_str[TMP_STR_LEN];
  char *s;
  float percent;
  int  error, point_found;
#ifdef FOO

  if ((fp = fopen(FILE_SYSTEMS_LIST, "r")) == NULL)
  {
    sprintf(error_str, "%s missing. No problem.", FILE_SYSTEMS_LIST);
    ERROR(' ', "", error_str);
    return;
  }

  i = 0;
  while (fgets(line, 80, fp))
  {
    /* Skip over comments (lines starting with a '#') */
    if (line[0] == '#')
      continue;

    sscanf(line, "%s %s\n", File_systems[i].name, s_threshold);

    rc = statfs(File_systems[i].name, &fs_buf);
    if (rc == 0)
    {
      sprintf(error_str, "FS: %s is not mounted", File_systems[i].name);
      ERROR(' ', "", error_str);
      continue;
    }
    else if (rc == -1)
    {
      sprintf(error_str, "FS: %s: error #%d", File_systems[i].name, errno);
      ERROR(' ', "", error_str);
      continue;
    }

    /* atof() doesn't work so we need to verify correctness
       of entry before passing it off to sscanf() */
    error = 0;
    point_found = 0;
    s = s_threshold;
    for (; *s; s ++)
    {
      if (!isdigit(*s) && *s != '.')
      {
	error = 1;
	break;
      }
      if (*s == '.')
      {
	if (point_found)
	{
	  error = 1;
	  break;
	}
	else
	  point_found = 1;
      }
    }

    if (error)
    {
      sprintf(error_str, "FS: %s: %s percent is invalid", File_systems[i].name, s_threshold);
      ERROR(' ', "", error_str);
      continue;
    }


    sscanf(s_threshold, "%f", &percent);
    percent /= 100.0;

    if (percent <= 0.0 || percent > 1.0)
    {
      sprintf(error_str, "FS: %s: %s percent is invalid", File_systems[i].name, s_threshold);
      ERROR(' ', "", error_str);
      continue;
    }

    File_systems[i].threshold = (int) ((1.0 - percent) * (float) fs_buf.fd_req.btot);

    i ++;
    if (i == MAX_FILE_SYSTEMS)
      break;
  }

  Num_file_systems = i;

  fclose(fp);
#endif
}

void
file_system_check()
{
  int  i, rc;
#ifdef FOO

  for (i = 0; i < Num_file_systems; i ++)
  {
    rc = statfs(File_systems[i].name, &fs_buf);
    if (rc == 1)
      if (fs_buf.fd_req.bfreen <= File_systems[i].threshold)
      {
	mk_except((struct calls *)NULL, (struct veh_driv *)NULL,
		  SYS_ERR, DISK_ALMOST_FULL, -1, File_systems[i].name);
      }
  }
#endif
}
