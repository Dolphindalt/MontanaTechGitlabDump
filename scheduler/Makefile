#
# CSCI460 Operating Systems
# Makefile for Building Processes Lab
#
# Phillip J. Curtiss, Associate Professor
# pcurtiss@mtech.edu, 406-496-4807
# Department of Computer Science, Montana Tech
#
# Revised 2018-04-04
#
# C-compiler set to x86 compiler
CC=	gcc
CFLAGS=	-std=gnu11 -g -pedantic -O0 -I ./includes -Wall -Werror \
	-Wno-unused-variable \
	-Wno-maybe-uninitialized \
	-Wno-declaration-after-statement \
	-DDEBUG=1
POSIX=	-lpthread
#
# Set all the targets you want to build
TARGET=	os-sim testReadyQueue
#
# Set the ALL rule to the project file(s) to build
all: $(TARGET)

# Implicit Rule - build object file from c source
.c.o:
	$(CC) $(CFLAGS) -c $<

# Explicit Rule - os-simulator with schedule algorithms
os-sim: os-sim.o process.o student.o readyQueue.o
	$(CC) $(CFLAGS) -o $@ $^ $(POSIX)

# Explicit Rule - test program for readyQueue ADT
testReadyQueue: test.o readyQueue.o
	$(CC) $(CFLAGS) -o $@ $^

# Dependencies
os-sim.o: os-sim.c os-sim.h 
process.o: process.c process.h 
student.o: student.c student.h
readyQueue.o: readyQueue.c readyQueue.h
test.o: test.c

#
# Set non-build rules below
.PHONY: clean

# Clean up project files
clean:
	$(RM) *.o $(TARGET)
