# Final binary name. This will be the name of the executable produced by calling
# `make` or `make <prog>`.
PROG	= occ


######################
# C Compiler Variables
######################

# Use gcc as the compiler for regular c code
CC	= gcc

# C compile time flags. Use C11 with optimization. Warn all.
# Include the pthreads library and posix source for signal handling.
CFLAGS	= -std=c11 -O3 -g -W -Wall -pthread -D_POSIX_SOURCE

# All C source files.
SOURCES	= \
	bitvec.c	\
	flow.c		\
	graph.c		\
	occ-enum2col.c	\
	occ-gray.c	\
	occ.c		\
	util.c	\

# Full compile command for C files. Use the C compiler with C
# compile time flags.
CCOMPILE = $(CC) $(CFLAGS)

# C object files. Just replace the .c extension with a .o extension
# for all source files.
OBJS	= $(SOURCES:.c=.o)


########################
# C++ Compiler Variables
########################

# C++ compiler Use clang++ by default.
CXX = clang++

# C++ source files.
CXX_SOURCES = \
	main.cpp \
	find_occ.cpp \
	heuristics/Debug.cpp\
	heuristics/Ensemble.cpp\
	heuristics/Graph.cpp\
	heuristics/Heuristics.cpp

# C++ object files. Just replace the .cpp extension with a .o
# extension for all source files.
CXX_OBJS = $(CXX_SOURCES:.cpp=.o)

# C++ compile flags. Use C++14 with optimization and warn all.
CXXFLAGS = -std=c++14 -Wall -g -O3


################
# Other Commands
################

# Command for linking object files. Use the C++ compiler and flags.
CLINK	= $(CXX) $(CXXFLAGS)


############
# Make Rules
############

# Make all. Default rule because it's placed first.
# Alias for `make depend` and `make <prog>`, and
# generates the final executable.
all: depend $(PROG)

# Make program. Depends on C and C++ object files
# and generates the final executable file.
$(PROG): $(OBJS) $(CXX_OBJS)
	$(CLINK) $(OBJS) $(CXX_OBJS) -o $(PROG)

# Depend. This is a shortcut for generating rules
# for object file dependencies. Generates a local
# file named .depend which is then included.
include .depend
.depend: depend
depend: $(SOURCES)
	$(CC) $(CFLAGS) -MM *.c > .depend
	$(CXX) $(CXXFLAGS) -MM $(CXX_SOURCES) >> .depend

# Rules for how to deal with object files with
# C and C++ source. Use the respective compiler
# to read in the file named `$<` and output
# an object file at $@.
%.o: %.c
	$(CCOMPILE) -c $< -o $@
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Make clean. Remove all the compile time junk.
clean:
	rm -rf $(PROG) $(OBJS) $(CXX_OBJS) core gmon.out .depend *.DSYM
