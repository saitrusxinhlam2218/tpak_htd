#--------------------------------------------------------
CC=xlc

include $(CISAMDB_DIR)/lib/cisamdb_make.inc

SRC=../..
INC=../include
INCDIRS= -I$(INC) -I../enhance_lib $(CISAMDB_INCL)
ENH=../enhance_lib

CFLAGS= -g -qnoro $(INCDIRS) -I$(ENH) $(I_AM_TESTING) -DHR24 -DINTERNATIONAL -DSCANDINAVIA
CC_FLAGS=$(CFLAGS) $(SUP_FLAGS) $(ADD_C_FLAGS)

ESQL=esql -static -g -qnoro $(INCDIRS)

.c.o :
	$(CC) -c $(CFLAGS) $<

