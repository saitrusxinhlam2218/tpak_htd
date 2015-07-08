ENH= ../enhance_lib
CFLAGS = -g -non_shared -I../include -I$(ENH) -DDEBUG -DPIPES -DHR24 -DINTERNATIONAL -DSCANDINAVIA $(LG) $(REL_FLAGS)
LIBFLAGS = -L../lib -L$(ENH) -lm -ldb -lisam -lfill -lcurses -lCall -lcommon -lFleet -lList -ldbenhance

all: znpoly_checksum_build znpoly_convert

CHECKSUM_BUILD_OBJ = znpoly_checksum_build.o

znpoly_checksum_build: $(CHECKSUM_BUILD_OBJ)
	cc -o znpoly_checksum_build $(CFLAGS) $(CHECKSUM_BUILD_OBJ) $(LIBFLAGS)

CONVERT_OBJ = znpoly_convert.o

znpoly_convert: $(CONVERT_OBJ)
	cc -o znpoly_convert $(CFLAGS) $(CONVERT_OBJ) $(LIBFLAGS)
