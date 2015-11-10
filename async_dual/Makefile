#
#  Makefile for async
#
#                 RCS INFO
#
#  @(#)  $RCSfile: Makefile,v $
#  @(#)  $Revision: 1.2 $
#  @(#)  $Date: 1999/07/21 17:37:42 $
#  @(#)  $Author: taxiadm $
#  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/async_dual/Makefile,v $
#
#  Copyright (c) 1991 - Motorola Mobile Data Division, Seattle, WA
#
#ident "@(#) head:$RCSfile: Makefile,v $	$Revision: 1.2 $"
#

include ../Makefile.inc


SRCS =	async.c \
	async_utl.c \
	getpar.c \
	ipc.c \
	parman.c \
	parman_utl.c \
	pminit.c \
	tcp_open.c \
	udp_link.c \
	hash.c

SRCS2 = hot.c

OBJS =	$(SRCS:.c=.o)
OBJS2 = $(SRCS2:.c=.o)
INC =	../include
LIB =	-L../lib -lcommon
PGM =	async
PGM2 = hot
DEST =	../../../usr/taxi/bin
all:	$(PGM) $(PGM2)

clean:
	rm -f $(PGM) $(OBJS)

install:	$(PGM)
	cp $(PGM) $(DEST)

$(PGM):	$(OBJS)
	$(CC) -g -o $(PGM) $(OBJS) $(EXT_OBJS) $(LIB)

$(PGM2): $(OBJS2)
	$(CC) -g -o $(PGM2) $(OBJS2)


hot.o:	hot.c

async.o:	$(INC)/IPCS.h $(INC)/STDIO.h $(INC)/acc.h $(INC)/async.h $(INC)/ipcpriority.h $(INC)/mmp.h \
		$(INC)/msg.h $(INC)/parman.h $(INC)/std.h /usr/include/ctype.h /usr/include/errno.h /usr/include/fcntl.h \
		/usr/include/sgtty.h /usr/include/signal.h /usr/include/stdio.h /usr/include/string.h /usr/include/sys/file.h \
		/usr/include/sys/ioctl.h /usr/include/sys/ipc.h /usr/include/sys/ttychars.h /usr/include/sys/ttydev.h \
		/usr/include/sys/types.h async.c

async_utl.o:	$(INC)/ASSERT.h $(INC)/STDIO.h $(INC)/acc.h $(INC)/async.h $(INC)/mmp.h $(INC)/std.h /usr/include/stdio.h \
		/usr/include/sys/ipc.h /usr/include/sys/sem.h /usr/include/sys/shm.h /usr/include/sys/types.h async_utl.c

getpar.o:	$(INC)/_parman.h $(INC)/parman.h $(INC)/std.h /usr/include/errno.h /usr/include/fcntl.h /usr/include/stdio.h \
		/usr/include/sys/file.h /usr/include/sys/ipc.h /usr/include/sys/sem.h /usr/include/sys/shm.h \
		/usr/include/sys/types.h /usr/include/unistd.h getpar.c



parman.o:	$(INC)/_parman.h $(INC)/parman.h $(INC)/std.h /usr/include/errno.h /usr/include/fcntl.h /usr/include/stdio.h \
		/usr/include/string.h /usr/include/sys/file.h /usr/include/sys/ipc.h /usr/include/sys/sem.h \
		/usr/include/sys/shm.h /usr/include/sys/types.h /usr/include/unistd.h parman.c

parman_utl.o:	$(INC)/STDIO.h $(INC)/parman.h $(INC)/std.h /usr/include/stdio.h parman_utl.c

pminit.o:	$(INC)/_parman.h $(INC)/parman.h $(INC)/std.h /usr/include/errno.h /usr/include/fcntl.h /usr/include/stdio.h \
		/usr/include/sys/file.h /usr/include/sys/ipc.h /usr/include/sys/sem.h /usr/include/sys/shm.h \
		/usr/include/sys/types.h /usr/include/unistd.h pminit.c
