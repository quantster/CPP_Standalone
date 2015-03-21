BOOST_ROOT = /Users/nitish/Library/Boost
INCDIR = -I. -I$(BOOST_ROOT)
LIBDIR= -L$(BOOST_ROOT)/lib

OBJDIR = ./

OBJFILES := $(patsubst %.cpp,%.o, $(wildcard *.cpp))

OBJS = $(addprefix $(OBJDIR), $(OBJFILES))

EXE := query_option_chains

BACKUPDIR = "/Users/nitish/Projects/lumacu.BAK"
DATESTAMP = `date "+%h_%d_%H_%M"`
BACKUP = $(BACKUPDIR)/$(DATESTAMP)_$(EXE).tar.gz

# compiler configuration
CC = g++

CFLAGS = -g -O3
# -c       compile only, generate corresponding .o files
# -g       generate code suitable for the debugger
# -p       generate code suitable for the profiler
# -O       attempt code optimizations

WFLAGS = 

LFLAGS = -lm -lboost_program_options -lboost_system

#DFLAGS= -DDEBUG

# targets
# compile sources
$(OBJDIR)%.o : %.cpp
	@echo "Compiling: " $<
	$(CC) -c $(CFLAGS) $(DFLAGS) $(WFLAGS) $(INCDIR) -o $@ $<

$(EXE) : $(OBJS)
	@echo "Linking: " $^
	$(CC) $(CFLAGS) $(DFLAGS) $(LFLAGS) $(LIBDIR) -o $(EXE) $^

clean :
	rm -f $(OBJS) $(EXE)

tar :
	tar zcf - $(MAKEFILE) $(wildcard *.cpp) $(wildcard *.h) > $(BACKUP)
	@echo "Created: " $(BACKUP)


.PHONY : clean tar

.DEFAULT : exe

