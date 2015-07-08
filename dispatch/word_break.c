static char sz__FILE__[]="@(#)word_break.c	10.1.2.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  word_break.c; Rel: 10.1.2.1; Get date: 5/11/95 at 10:16:34
* @(#)  Last delta: 12/2/94 at 18:36:00
* @(#)  SCCS File: /taxi/sccs/s.word_break.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/


#include <stdio.h>
#include <strings.h>
#include <ctype.h>
#include "tmplen.h"


static char * get_word(char * p)
{

	register char * t;
	static char temp[TMP_STR_LEN];

	t = strchr(p,' ');
	if (t)
	{
		*t = '\0';
		strcpy(temp,p);
		*t = ' ';
		t++;
		p = t;
	}
	else
		return(t);
	return(temp);
}

char * word_break(char * string, int line_length, char * line_break)
{

	
	register char * p , * s , * word;
	int x, len, curline, ii, mark;
	static char lines[TMP_STR_LEN];
	
	p = string;
	
	len = strlen(p);
	s = lines;
	mark = curline = 0;
	
	while (mark < len)
	{
		if ((word = get_word(p)) != NULL)
		{
			x = strlen(word);
			if (curline + x < line_length)
			{
				ii = stcp(s,word,x);
				s += ii;
				*s++=' ';
			}
			else 
			{
				if (x < line_length)
				{
				if (curline != line_length)
				{	
					if (curline + x > line_length)
					{
						ii = stcp(s,line_break,strlen(line_break));
						s+= ii;
/*
					*s++=line_break;
*/
					}
				}
				}
				ii = stcp(s,word,x);
				s += ii;
				*s++=' ';
				curline = 0;	
				
			}
			curline+=(x + 1);
#ifdef NAMERICA
			if (curline >= line_length)
			  {
			    ii = stcp(s, line_break, strlen(line_break));
			    s+= ii;
			    curline = 0;
			  }
#endif
			p +=(x + 1);
			mark +=(x + 1);
		}
		else 
		{
			x = strlen(p);
			if (mark < len)
			{
				if (curline + x < line_length)
				{
					ii = stcp(s,p,strlen(p));
					s += ii;
					ii = stcp(s,line_break,strlen(line_break));
					s+= ii;
				}
                        	else 
                        	{   
                               		if (x < line_length)
                                	{
                                		if (curline != line_length)
                                		{       
                                        		if (curline + x > line_length)
                                        		{
       
                                                		ii = stcp(s,line_break,strlen(line_break));
                                                		s+= ii;
                                        		}
                                		}
                                	}
                                	ii = stcp(s,p,x);
                                	s += ii;
				}
      
/*
				*s++=line_break;
*/
			}
			*s++='\0';
			break;
			
		}
			
	
		
	}
	*s++='\0';
	return(lines);
}

char * unword_break(char * str, char * line_break)
{
	register char * p , * s , * word;
	int x, len, curline, ii;
	static char lines[TMP_STR_LEN];
	char * find_str();

	s = str;
	lines[0] = '\0';
	while (p = find_str(s,line_break))
	{

		*p++= '\0';
		*p++= ' ';
		 strcat(lines,s);
		 s = p;
	}

	return(lines);
}

