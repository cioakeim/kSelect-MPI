
SRCDIR=src
OBJDIR=obj
BINDIR=bin
INCDIR=include

SRC=$(wildcard $(SRCDIR)/*.c) 
OBJ=$(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
INC=$(wildcard $(INCDIR)/*.h)
BIN=$(BINDIR)/main

CC=mpicc # Used for Open MPI.
CFLAGS= -Wall -Wextra -Iinclude -g

list:
	@echo $(SRC)
	@echo $(INC)
	@echo $(OBJ)

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJ)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# If there is a header file for that source code, it's a dependency.
$(SRCDIR)/%.c: $(INCDIR)/%.h
	@touch $@ 

clean:
	rm -rf $(BIN) $(OBJ)

