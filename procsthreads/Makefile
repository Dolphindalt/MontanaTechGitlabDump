#
# Makefile for Generating Eexecutables
#
# CSCI 460 - Operating Systems
#
# Phillip J. Curtiss, Associate Professor
# pcurtiss@mtech.edu, 406-496-4807
# Computer Science Department, Montana Tech
#
# Project Files Specified Below
##
# Project Parts
PARTS := iterative process thread
#
# Variables to Build genData executable
genData-SRCS := genData.c
genData-OBJS := $(genData-SRCS:.c=.o)
genData-DEPS := $(genData-SRCS:.c=.dep)
#
# Variables to Build main-iterative executable
$(word 1,${PARTS})-SRCS := main.c $(wildcard $(word 1,${PARTS})/*.c)
$(word 1,${PARTS})-OBJS := \
	$(subst main.o,main-$(word 1,${PARTS}).o,$($(word 1,${PARTS})-SRCS:.c=.o))
$(word 1,${PARTS})-DEPS := $($(word 1,${PARTS})-SRCS:.c=.dep)
#
# Variables to Build main-process executable
$(word 2,${PARTS})-SRCS := main.c $(wildcard $(word 2,${PARTS})/*.c)
$(word 2,${PARTS})-OBJS := \
	$(subst main.o,main-$(word 2,${PARTS}).o,$($(word 2,${PARTS})-SRCS:.c=.o))
$(word 2,${PARTS})-DEPS := $($(word 2,${PARTS})-SRCS:.c=.dep)
#
# Variables to Build main-thread executables
$(word 3,${PARTS})-SRCS := main.c $(wildcard $(word 3,${PARTS})/*.c)
$(word 3,${PARTS})-OBJS := \
	$(subst main.o,main-$(word 3,${PARTS}).o,$($(word 3,${PARTS})-SRCS:.c=.o))
$(word 3,${PARTS})-DEPS := $($(word 3,${PARTS})-SRCS:.c=.dep)
#
# Aggregated object files 
OBJS := $(genData-OBJS) $(foreach part, ${PARTS}, $(${part}-OBJS))
# Aggregated dependency files
DEPS :=	$(genData-DEPS) $(foreach part, ${PARTS}, $(${part}-DEPS))
# List all targets this Makefile is to create from source files
TARGET := $(foreach part, ${PARTS}, main-${part}) genData

###################################################
# Should Not need to modify lines of Makefile below
#
CC := gcc
# Compiler flags; level-3 optimization; comform to C standard
CFLAGS := -O3 -pedantic-errors -Wall -g -Werror \
								     -Wno-pointer-arith \
								     -Wno-unused-variable \
								     -Wno-unused-but-set-variable
# Additional Library Search Paths specified below
LDFLAGS := 
# Additional Libraries to link in with executable below
LDLIBS := -lrt -lpthread

#
# Provide Make with additionally known suffixes
.SUFFIXES:	.dia .dep .tex .md

# Default rule to make if no target specified
# (make dependencies first, then all targets)
default: $(DEPS) $(TARGET) 

# Rules to generate targets
# Note: If more than one target, might need multiple rules
#       of the form below
genData: $(genData-OBJS)
	$(CC) -o $@ $^

main-iterative: $(iterative-OBJS)
	$(CC) -o $@ $^ $(LDLIBS)

main-process: $(process-OBJS)
	$(CC) -o $@ $^ $(LDLIBS)

main-thread: $(thread-OBJS)
	$(CC) -o $@ $^ $(LDLIBS)

# Generate main for each part of the project from the 
# same main.c file using a define to include/exclude
# code within main.c appropraite for project part
main-iterative.o: main.c
	$(CC) $(CFLAGS) -DITERATIVE -o $@ -c $^ 

main-process.o: main.c
	$(CC) $(CFLAGS) -DPROCESS -o $@ -c $^

main-thread.o: main.c
	$(CC) $(CFLAGS) -DTHREAD -o $@ -c $^

# Make dependency file for each source file
# include all dependency files so Make can 
# conditionally make required project file
%.dep: %.c
	$(CC) -MM $< -o $@
-include $(DEPS)

#
# Clean up Intermediate files and targets
.PHONY: clean
clean: 
	-$(RM) $(DEPS) $(OBJS) $(TARGET)

