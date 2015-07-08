PHONY: it lib

CFLAGS=-std -DANSIC
LDFLAGS=
LIBES=-lm

it: lib wgstest

wgstest: wgstest.o $(lib)
	$(CC) $(CFLAGS) -o $@ wgstest.o wgstrans.o \
	transform.o wgs84.o datumTrans.o bessel.o $(LIBES)

wgstest.o: wgstest.c wgstrans.h

lib: bessel.o datumTrans.o transform.o wgs84.o wgstrans.o

bessel.o: bessel.c bessel.h

datumTrans.o: datumTrans.c bessel.h

wgs84.o: wgs84.c wgs84.h 

transform.o: transform.c transform.h datumTrans.h bessel.h wgs84.h

wgstrans.o: wgstrans.c wgstrans.h transform.h
