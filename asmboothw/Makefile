##
# Makefile for generating Boot loader using GNU assmebly
# Assembly must conform to AT&T format
# (as opposed to Intel symtax - via nasm)
#
# CSCI 460 Operating Systems
#
##
# Set the linker to generate straight binary file - one instruction after
# the other - plain binary file
LDFLAGS := --oformat binary
##
# Inform the linker where our program starts by specifying the start label
ENTRY := __start
##
# Linker will attempt to relocate data setment assets
# informing the linker of the starting address of the program will cause
# it to use this address for relocation and not exceed memory limits
ADDR := 0x7c00
##
# must add .bin as a valid suffix for files Make is to consider
.SUFFIXES: .bin

##
# implicit rule to make a .o file from a .s file
# the $@ is the target and the $< is the list of dependencies
# $(AS) is the assmbler - default is gnu assembler
.s.o:
	$(AS) -o $@ $<

##
# implicit rule to make a .bin file from a .o file
# the $@ is the target and the $< is the list of dependencies
# $(LD) is the linker - default system linker
.o.bin:
	$(LD) -o $@ $(LDFLAGS) -e $(ENTRY) -Ttext $(ADDR) $<

# Note the above will generate a file (the .bin) and automatically 
# set the execute permission on the file - even though it is not an
# ELF executable for the default runtime
##

##
# the following targets do not generate any outputs
.PHONY: clean

##
# rule to remove intermediate files (e.g. .o .bin)
clean:
	$(RM) *.o *.bin

