#
# Makefile for Generating executables
#
# F19 CSCI 466 Networks
#
# Phillip J. Curtiss, Associate Professor
# Computer Science Department, Montana Tech
#
# Proj01-CtriviaClient
#
# Specify the Course Below
COURSE = F19 CSCI466 Networks
# Specify the Project Name below
PROJ =	Proj01-CtriviaClient: C Language Trivia Client
# Specify the assignment filename
ASGNMT = assignment.tex
#
# Project Files Specified Below
##
# Source files should be added to the SRCS variable
SRCS =	triviaClient.c
# Object files will be inferred from soruce files; adjust as needed
OBJS = $(SRCS:.c=.o)
# Dependency files will be inferred from source files; adjust as needed
DEPS =	$(SRCS:.c=.dep)
# List all targets this Makefile is to create from source files
TARGET = triviaClient

###################################################
# Should Not need to modify lines of Makefile below
#
CC = gcc
# gcc compiler flags; level-3 optimization
CFLAGS = -O3 -Wall -Werror
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

#
# Generate Documentation for project files
Doxyfile:
	-doxygen -g
	-sed -r -i 's/(^PROJECT_NAME\s*=).*$$/\1 \"$(COURSE)\"/' $@
	-sed -r -i 's/(^PROJECT_BRIEF\s*=).*$$/\1 \"$(PROJ)\"/' $@
	-sed -r -i 's/(^OUTPUT_DIRECTORY\s*=).*$$/\1 doc/' $@
	-sed -r -i 's/(^GENERATE_TREEVIEW\s*=).*$$/\1 YES/' $@

doxygen: Doxyfile
	-doxygen $^

#
# Generate README.md markdown from the assignment sheet
README.md: assignment/$(ASGNMT)
	-pandoc -f latex -t markdown --latexmathml -s -o README.md $^

