

IN2=../include
CM = ../common
MN = ../monitor
ENH=../enhance_lib

CFLAGS =  -g -non_shared -DHR24 -DINTERNATIONAL -DNDEBUG -I$(IN2) -I$(ENH) -g -DTRACE_FLAT $(INTFLAGS) $(LG)

OBJ = CallProbe.o
OBJ2 = gps_disp_read.o
OBJ3 = PlaceName_update.o
OBJ4 = autobook_nbrs.o
OBJ5 = CityNav_query_coords.o
OBJ6 = ShapeRead.o
OBJ7 = GPS_track.o
OBJ8 = GPS_track_server.o
OBJ9 = GPS_messaging.o
OBJ10 = TLayer_purge.o
OBJ11 = ShapetoStreet.o
OBJ12 = ShapeMatch.o
OBJ13 = geo_mod.o parsecfg.o
OBJ14 = StreetImport.o
OBJ15 = GeoAddrImport.o

all:	autobook_nbrs CityNav_query_coords GPS_track TLayer_purge

TLayer_purge:	$(OBJ10)
	cc -o TLayer_purge $(CFLAGS) $(OBJ10) -L../lib -ldb -lfill -lrate -lisam

PlaceName_update:	$(OBJ3)
	cc -o PlaceName_update $(CFLAGS) $(OBJ3) -L../lib -ldb -lfill -lrate -lisam

CityNav_query_coords:	$(OBJ5)
	cc -o CityNav_query_coords $(CFLAGS) $(OBJ5) -L../lib -ldb -lfill -lrate -lisam -lcnapi

ShapeRead:	$(OBJ6)
	cc -o ShapeRead $(CFLAGS) $(OBJ6) -L../lib -ldb -lfill -lrate -lisam

ShapetoStreet:	$(OBJ11)
	cc -o ShapetoStreet $(CFLAGS) $(OBJ11) -L../lib -ldb -lfill -lrate -lisam -lcommon -lm

StreetImport:	$(OBJ14)
	cc -o StreetImport $(CFLAGS) $(OBJ14) -L../lib -ldb -lfill -lrate -lisam -lcommon -lm

GeoAddrImport:	$(OBJ15)
	cc -o GeoAddrImport $(CFLAGS) $(OBJ15) -L../lib -ldb -lfill -lrate -lisam -lcommon -lm

ShapeMatch:	$(OBJ12)
	cc -o ShapeMatch $(CFLAGS) $(OBJ12) -L../lib -ldb -lfill -lrate -lisam -lcommon

CallProbe:	$(OBJ)
	cc -o CallProbe $(CFLAGS) $(OBJ)

GPS_messaging:	$(OBJ9)
	cc -o GPS_messaging $(CLFAGS) $(OBJ9) -L../lib -lm -lmmp -lcommon

GPS_track:	$(OBJ7)
	cc -o GPS_track $(CFLAGS) $(OBJ7) -L../lib -ldb -lfill -lrate -lisam

GPS_track_server:	$(OBJ8)
	cc -o GPS_track_server $(CFLAGS) $(OBJ8) -L../lib -ldb -lfill -lrate -lisam -lcommon -lm


gps_disp_read:	$(OBJ2)
	cc -o gps_disp_read $(CFLAGS) $(OBJ2) -L../lib -ldb -lfill -lrate -lisam

autobook_nbrs:	$(OBJ4)
	cc -o autobook_nbrs $(CFLAGS) $(OBJ4)
geo_mod:	$(OBJ13)
	cc -o geo_mod $(CFLAGS) $(OBJ13) -L../lib -ldb -lfill -lrate -lisam -lcommon -lm

