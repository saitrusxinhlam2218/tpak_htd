#/****************************************************************************
#*                                                                           *
#*                           S C C S   I N F O                               *
#*                                                                           *
#* @(#)  m_catgets.mk; Rel: 10.1.0.0; Get date: 4/18/95 at 16:33:06
#* @(#)  Last delta: 12/2/94 at 12:02:02
#* @(#)  SCCS File: /taxi/sccs/s.m_catgets.mk
#*                                                                           *
#* Copyright (c) 1990 - MDI Seattle                                          *
#*                                                                           *
#*****************************************************************************/
#ident "@(#) head:m_catgets.mk	10.1.0.0"


CFLAGS = -g

m_catgets.o : m_catgets.c
	cc -c -o m_catgets.o m_catgets.c 
	@echo "m_catgets updated"


