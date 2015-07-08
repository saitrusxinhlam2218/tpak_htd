static char sz__FILE__[]="@(#)scanterm.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  scanterm.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:33:13
 * @(#)  Last delta: 12/2/94 at 11:42:48
 * @(#)  SCCS File: /taxi/sccs/s.scanterm.c
 *                                                                           *
 *****************************************************************************/
#include <curses.h>
#include <strings.h>

#include "comm_strdefs.h"
#include "language.h"
#define TOKENIZE_SRC
#include "tokenize.h"

char Terminal_name[32];

read_string(start_of_string, len, line, str, max_str_len)
int start_of_string;  /* position on line where string starts */
int len;		/* Length line read in */
char *line;		/* The line read in */
char *str;		/* Where to store the string */
int  max_str_len;	/* How much space there is in str */
{
  int j, k;

  k = 0;
  for (j = start_of_string; j < len; j ++) {
    
    /* If no more room for string, stop. */
    if (k == max_str_len) 
      break;
    
    if (line[j] == '\\') {
      
      /* If last char on line, then ignore it. */
      if (j == len - 1)
	break;
      
      switch (line[++j]) {
      case 'E':
      case 'e':
	str[k ++] = ESC;
	break;
	
      case '\\':
	str[k ++] = '\\';
	break;
	
      case 'R':
      case 'r':
	str[k ++] = '\r';
	break;

      case 'N':
      case 'n':
	str[k ++] = '\n';

      default:
	break;
      }
    }
    else {
      str[k ++] = line[j];
    }
  }
  
  str[k] = '\0';
  
  /* Return length of string */
  return(k);

}

term_setup(term)
     char *term;
{
  char termfile[100];
  char cap[MAX_KEY_CODE_LEN + 1];
  char line[MAX_LINE_LENGTH + 1];
  int  i, j, k, m, n;
  int  start_of_string;
  int  len;
  int  line_num;
  int  found_cap;
  FILE *fp;

  /** For Stockholm only -- temporary workaround **/
  if ( !strncmp( LANGUAGE, "Stockholm", 9 ) )
    {
      if ( !getenv("UI_8BIT") )
	strcpy( term, "vt100-w" );
    }

  strcpy(Terminal_name, term);

  /* First check TERM_DIR environment variable, then standard location */
  sprintf(termfile, "%s/%c/%s", (char *)getenv("TERM_DIR"), term[0], term);
  if ((fp = fopen(termfile, "r")) == NULL)
    {
      sprintf(termfile, "%s/%c/%s", TERM_DIR, term[0], term);
      if ((fp = fopen(termfile, "r")) == NULL)
	{
	  fprintf(stderr, "Unable to open file (%s).\n", termfile);
#ifdef FOO
	  fprintf(stderr, catgets(COMMON_m_catd, COMMON_SET, COMMON_TERM_OPEN,
				  "Unable to open file (%s).\n"), termfile);
#endif
	  return(ERR);
	}
    }
  
  n = -1;
  line_num = 0;
  while (fgets(line, MAX_LINE_LENGTH, fp) != NULL)
    {

      line_num ++;
      
      /* '#' starts a comment line */
      if (line[0] == '#')
	continue;
      
      i = 0;
      
      len = strlen(line);
      /* fgets adds \n to end. Minimum length of line is then: min length
       * of cap mnemonic (2 char), = (1 char), keycode (1 char), \n (1)
       * for total of 5 */
      if (len <= 5)
	continue;
      
      /* Get rid of \n at end */
      line[--len] = '\0';
      
      /* Find the capabilty name */
      k = 0;
      while (i < len && line[i] != '=' && k < MAX_KEY_CODE_LEN)
	cap[k ++] = line[i ++];
      cap[k] = '\0';
      start_of_string = i + 1;
      
      /* Skip over lines without an '=' */
      if (line[i ++] != '=')
	continue;
      
      /* Go thru list of capabilty names to find match */
      m = -1;
      found_cap = 0;
      while (Caps[++m].capname[0])
	{
	  
	  if (strcmp(cap, Caps[m].capname) == 0)
	    {
	      found_cap = 1;
	      
	      /* Function keyes will have codes more than 0 */
	      if (Caps[m].code >= 0)
		{
		  n ++;
		  strcpy(Fk[n].name, Caps[m].capname);
		  Fk[n].code = Caps[m].code;
		  
		  len = read_string(start_of_string, len, line, Fk[n].s,
				    MAX_KEY_CODE_LEN);
		  Fk[n].len = len;
		  if (Max_func_key_len < Fk[n].len)
		    Max_func_key_len = Fk[n].len;
		  break;
		} /* if */
	      
	      /* Non function key */
	      else
		{
		  if (strcmp(cap, "is") == 0)
		    {
		      Init_terminal = (char *) malloc(len);
		      read_string(start_of_string, len, line, Init_terminal,
				  len);
		    }
		  else if (strcmp(cap, "ts") == 0)
		    {
		      Status_line_start = (char *) malloc(len);
		      read_string(start_of_string, len, line, Status_line_start,
				  len);
		    }
		  else if (strcmp(cap, "fs") == 0)
		    {
		      Status_line_end = (char *) malloc(len);
		      read_string(start_of_string, len, line, Status_line_end,
				  len);
		    }
		} /* else */
	    } /* if */
	} /* while */
      if (!found_cap)
	{
	  fprintf(stderr, "%s: ERROR line %d - '%s'\n", termfile, line_num, line);
	}
    } /* while */
    
  N_func_keys = n + 1;
  fclose(fp);
  return(0);
}
  
/*
  initialize_terminal() -
  The terminal should 132 columns by 24 lines.
  Use the Init_terminal string if it is read in
  by term_setup(), otherwise, send some default
  settings for DEC/ANSI terminals.
*/
config_tty()
{

  /* Wyse 60 terminals which we run as vt100w's,
     Suns, and vt3xx are known to have the
     following Status line information. Other
     terminals can be defined in term file */
  
  if (strcmp(Terminal_name, "vt100w") == 0 ||
      strcmp(Terminal_name, "vt100-w") == 0)
    {
      if (Status_line_start == NULL)
	{
	  Status_line_start = "\033[>,\027";
	  Status_line_end = "\027";
	}
    }
  
  if (strcmp(Terminal_name, "vt300w") == 0 ||
      strcmp(Terminal_name, "vt300-w") == 0)
    {
      if (Status_line_start == NULL)
	{
	  Status_line_start = "\033[1$}\r";
	  Status_line_end = "\033[0$}";
	}
    }
  else if (strcmp(Terminal_name, "sun") == 0 ||
	   strcmp(Terminal_name, "sunw") == 0)
    {
      if (Status_line_start == NULL)
	{
	  Status_line_start = "\033]l";
	  Status_line_end = "\033\\";
	}
    }
  
  if (Init_terminal == NULL)
    {
      /* If not defined default to the following: */

      /* the following ANSI strings are for vt100, tm220, and other ANSI terminals */
      printf("\033\076"); 	/* keypad normal mode */
      printf("\033[?1l");	/* normal arrow keys */
      printf("\033[40l");	/* 24 lines */
      printf("\033[?3h");	/* 132 columns */
    }
  else
    printf("%s", Init_terminal);

#ifdef FOO
  printf("\033[?42h");
  printf("\033(7");
#endif

  fflush(stdout);

}
