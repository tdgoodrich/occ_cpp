CC	= gcc
CFLAGS	= -std=c99 -O3 -g -W -Wall
# disable internal consistency checking for moderate (10-30%) speedup
#CFLAGS  += -DNDEBUG

PROG	= occ

LDPATH	=
LIBS	=

SOURCES	= \
	bitvec.c	\
	flow.c		\
	graph.c		\
	main.c		\
	occ-enum2col.c	\
	occ-gray.c	\
	occ.c		\
	util.c


CCOMPILE = $(CC) $(CFLAGS)
CLINK	= $(CC) $(CFLAGS) $(LDPATH) $(LIBS)

OBJS	= $(SOURCES:.c=.o)

all: depend $(PROG)

%.o: %.c
	$(CCOMPILE) -c $<

$(PROG): $(OBJS)
	$(CLINK) $(OBJS) -o $(PROG)

clean:
	rm -f $(PROG) $(OBJS) core gmon.out

VER	= 1.1

dist: all
	rm -rf occ-$(VER)
	mkdir occ-$(VER)
	cp COPYING README occ-$(VER)
	cp Makefile *.c *.h occ-$(VER)
	mkdir -p occ-$(VER)/data/japanese
	mkdir -p occ-$(VER)/data/afro-americans
	cp data/japanese/*.graph occ-$(VER)/data/japanese
	cp data/afro-americans/*.graph occ-$(VER)/data/afro-americans
	mkdir occ-$(VER)/synth-data
	cp synth-data/*.ml synth-data/*.mli synth-data/Makefile occ-$(VER)/synth-data
	GZIP=--best tar -cvvzf occ-$(VER).tar.gz occ-$(VER)
	cp occ-lp.py occ-$(VER)

realclean: clean
	rm -f *~ *.bak

.depend: depend

depend: $(SOURCES)
	$(CC) $(CFLAGS) -MM *.c > .depend

include .depend
