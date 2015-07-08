#***********************************************************************
#                 RCS INFO
#
#  @(#)  $RCSfile: Makefile,v $
#  @(#)  $Revision: 1.4 $
#  @(#)  $Date: 1999/05/07 18:05:04 $
#  @(#)  $Author: taxiadm $
#  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/db/Makefile,v $
#
#  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
#
#**********************************************************************/
#ident "@(#) head:$RCSfile: Makefile,v $	$Revision: 1.4 $"

#static char rcsid[] = "$Id: Makefile,v 1.4 1999/05/07 18:05:04 taxiadm Exp $";

include ../Makefile.inc

SRC =..


OFILES = \
db.o \
db_new.o 

libdb.a : $(OFILES)
	ar ru libdb.a $(OFILES) ;\
	ranlib libdb.a ;\
	cp libdb.a $(SRC)/lib

clean :
	rm $(OFILES) libdb.a ../lib/libdb.a



