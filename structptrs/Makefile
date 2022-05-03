#
# Makefile for Generating C executables
#
# CSCI460 - Operating Systems
#
# Phillip J. Curtiss, Associate Professor
# Computer Science Department, Montana Tech
# Museum Buildings, Room 208 
#
# Project Files Specified Below
##
# Source files should be added to the SRCS variable
SRCS =	main.c Message.c Tree.c
# Object files will be inferred from soruce files; adjust as needed
OBJS =	$(SRCS:.c=.o)
# Dependency files will be inferred from source files; adjust as needed
DEPS =	$(SRCS:.c=.dep)
# List all targets this Makefile is to create from source files
TARGET = main

###################################################
# Should Not need to modify lines of Makefile below
#
# g++ compiler flags; standard is C++14; level-3 optimization
CFLAGS = -O3 -Wall -Werror -Wno-return-type -g
# Additional Library Search Paths specified below
LDFLAGS = 
# Additional Libraries to link in with executable below
LDLIBS = 

#
# Provide Make with additionally known suffixes
.SUFFIXES:	.dia .dep .tex .md

# Default rule to make if no target specified
# (make dependencies first, then all targets)
default: $(DEPS) $(TARGET) 

# Rules to generate targets
# Note: If more than one target, might need multiple rules
#       of the form below
$(TARGET): $(OBJS)
	$(CC) -o $@ $^

# Make dependency file for each source file
# include all dependency files so Make can 
# conditionally make required project file
%.dep: %.c
	$(CC) -MM $< -o $@
-include $(DEPS)

#
# Clean up Intermediate files and targets
.PHONY: clean doxygen
clean: 
	-$(RM) $(DEPS) $(OBJS) $(TARGET)
