include ../Makefile.inc
SRC=../..
INC=$(SRC)/include
#INCDIRS=-I$(INC)/$(LANGUAGE) -I$(INC)
LIB = ..
#CFLAGS = $(INCDIRS) -DINTERNATIONAL -g $(INTFLAGS) $(REL_FLAGS)

SRCS = 	LL.c

OBJS = $(SRCS:.c=.o)

all: libLL.a

clean:
	rm -f libLL.a $(OBJS)

depend:
	makedep -m $(INCDIRS) -r $(SRCS)


libLL.a: $(OBJS)
	ar ru libLL.a $(OBJS) ;\
	ranlib libLL.a ;\
	cp libLL.a $(SRC)/lib


