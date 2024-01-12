
SRCDIR=src
OBJDIR=obj
BINDIR=bin
INCDIR=include
MAINSDIR=main
MAINOBJDIR=mainobj

SRC=$(wildcard $(SRCDIR)/*.c) 
OBJ=$(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
INC=$(wildcard $(INCDIR)/*.h)
BIN=$(BINDIR)/main
MAINS=$(wildcard $(MAINSDIR)/*.c)

CC=mpicc # Used for Open MPI.
CFLAGS= -Wall -Wextra -Iinclude -g -lcurl

list:
	@echo $(SRC)
	@echo $(INC)
	@echo $(OBJ)

.PHONY: all clean

all: testing timeN timeP 

testing: $(MAINOBJDIR)/testing_job.o $(OBJ)
	@mkdir -p $(BINDIR)
	$(CC) -o $(BINDIR)/testing_job $^ $(CFLAGS)

timeN: $(MAINOBJDIR)/time_n_job.o $(OBJ)
	@mkdir -p $(BINDIR)
	$(CC) -o $(BINDIR)/time_n_job $^ $(CFLAGS)

timeP: $(MAINOBJDIR)/time_p_job.o $(OBJ)
	@mkdir -p $(BINDIR)
	$(CC) -o $(BINDIR)/time_p_job $^ $(CFLAGS)

main: $(MAINOBJDIR)/main.o $(OBJ)
	@mkdir -p $(BINDIR)
	$(CC) -o $(BINDIR)/main $^ $(CFLAGS) 

percentiles: $(MAINOBJDIR)/find_even_percentiles.o $(OBJ)
	@mkdir -p $(BINDIR)
	$(CC) -o $(BINDIR)/find_even_percentiles $^ $(CFLAGS)

$(MAINOBJDIR)/%.o: $(MAINSDIR)/%.c 
	@mkdir -p $(MAINOBJDIR)
	$(CC) -c $< -o $@ $(CFLAGS)  

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) -c $< -o $@ $(CFLAGS)  

# If there is a header file for that source code, it's a dependency.
$(SRCDIR)/%.c: $(INCDIR)/%.h
	@touch $@ 

clean:
	rm -rf $(OBJDIR)/* $(MAINOBJDIR)/* $(BINDIR)/*

