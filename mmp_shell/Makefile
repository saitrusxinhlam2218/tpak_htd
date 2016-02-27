include ../Makefile.inc

ENH=../enhance_lib
LIBFLAGS = -L../lib -L$(ENH) -lm -ldb -lfill -lcurses -lmmp -lCall -lcommon -lFleet -lList -ldbenhance $(CISAMDB_LIBS)

OBJ = mmp_shell.o

mmp_shell: $(OBJ)
	xlc $(CFLAGS) $(OBJ) $(LIBFLAGS) -o mmp_shell
