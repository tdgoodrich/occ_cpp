CC	= gcc
CFLAGS	= -std=c11 -O3 -g -W -Wall -pthread -D_POSIX_SOURCE
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


CPP_COMPILER = clang++
HEURISTICS_FILES = \
	heuristics/heuristic_solver.cpp\
	heuristics/Debug.cpp\
	heuristics/Ensemble.cpp\
	heuristics/Graph.cpp\
	heuristics/Heuristics.cpp
CPP_VERSION = -std=c++14
CPP_COMPILE_OPTIONS = -Wall -g -O3 -pthread
HEURISTICS_OUTPUT = heuristic_solver

all: heuristics_ensemble depend $(PROG)

heuristics_ensemble:
	$(CPP_COMPILER) $(CPP_VERSION) $(HEURISTICS_FILES) -o $(HEURISTICS_OUTPUT) $(CPP_COMPILE_OPTIONS)

%.o: %.c
	$(CCOMPILE) -c $<

$(PROG): $(OBJS)
	$(CLINK) $(OBJS) -o $(PROG)

clean:
	rm -f $(PROG) $(OBJS) core gmon.out $(HEURISTICS_OUTPUT)

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

run:
	./occ < data/afro-americans/32.graph -b


include .depend
