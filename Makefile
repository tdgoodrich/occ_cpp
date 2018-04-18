CC	= gcc
CFLAGS	= -std=c11 -O3 -g -W -Wall -pthread -D_POSIX_SOURCE
SOURCES	= \
	bitvec.c	\
	flow.c		\
	graph.c		\
	occ-enum2col.c	\
	occ-gray.c	\
	occ.c		\
	util.c	\
	find_occ.c
CCOMPILE = $(CC) $(CFLAGS)
OBJS	= $(SOURCES:.c=.o)


CPP_COMPILER = clang++
CPP_FILES = \
	main.cpp \
	heuristics/heuristic_solver.cpp\
	heuristics/Debug.cpp\
	heuristics/Ensemble.cpp\
	heuristics/Graph.cpp\
	heuristics/Heuristics.cpp
CPP_OBJS = $(CPP_FILES:.cpp=.o)
CPP_VERSION = -std=c++14
CPP_COMPILE_OPTIONS = -Wall -g -O3 -pthread

CLINK	= $(CPP_COMPILER) $(CPP_VERSION) $(CPP_COMPILE_OPTIONS)
PROG	= occ

VER	= 1.1

all: depend $(PROG)

$(PROG): $(OBJS) $(CPP_OBJS)
	$(CLINK) $(OBJS) $(CPP_OBJS) -o $(PROG)

include .depend
.depend: depend
depend: $(SOURCES)
	$(CC) $(CFLAGS) -MM *.c > .depend
	$(CPP_COMPILER) $(CPP_VERSION) -MM $(CPP_FILES) >> .depend

%.o: %.c
	$(CCOMPILE) -c $<

%.o: %.cpp
	$(CPP_COMPILER) $(CPP_VERSION) -c $< -o $@

clean:
	rm -f $(PROG) $(OBJS) $(CPP_OBJS) core gmon.out .depend
