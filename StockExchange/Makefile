INCDIR = -I. -I/Users/nitish/Library/Boost
LIBDIR= 

OBJDIR = ./

OBJFILES := $(patsubst %.cpp,%.o, $(wildcard *.cpp))

OBJS = $(addprefix $(OBJDIR), $(OBJFILES))

EXE := tibra

BACKUPDIR = "/Users/nitish/Projects/tibra.BAK"
DATESTAMP = `date "+%h_%d_%H_%M"`
BACKUP = $(BACKUPDIR)/$(DATESTAMP)_$(EXE).tar.gz

# compiler configuration
CC = g++

CFLAGS = -g -O3
# -c       compile only, generate corresponding .o files
# -g       generate code suitable for the debugger
# -p       generate code suitable for the profiler
# -O       attempt code optimizations

WFLAGS = -Wall

LFLAGS = -lm 

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

