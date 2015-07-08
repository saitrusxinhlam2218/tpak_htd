#***********************************************************************
#                 RCS INFO
#
#  @(#)  $RCSfile: Makefile,v $
#  @(#)  $Revision: 1.4 $
#  @(#)  $Date: 1998/02/23 21:48:38 $
#  @(#)  $Author: taxiadm $
#  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill_rate/Makefile,v $
#
#  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
#
#**********************************************************************/
#ident "@(#) head:$RCSfile: Makefile,v $	$Revision: 1.4 $"

#static char rcsid[] = "$Id: Makefile,v 1.4 1998/02/23 21:48:38 taxiadm Exp $";

SRC =..
IN1 = /usr/include
IN2=../include
CFLAGS = -I$(IN2)
OBJS =   trans_category.o tperiods.o trip_data.o conditions.o seasons.o relations.o relation_data.o \
	std_relations.o holidays.o cond_type.o m_action.o dist_filter.o

librate.a : $(OBJS) $(IN2)/mads_types.h $(IN2)/mads_isam.h $(IN2)/fleet.h $(IN2)/df_maint.h
	ar ru librate.a $(OBJS) ;\
	ranlib librate.a ;\
	cp librate.a $(SRC)/lib 
