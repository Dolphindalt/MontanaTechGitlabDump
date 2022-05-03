##
# CSCI 460 Operating Systems
#
# Classic Hello World Assembly Language Example
##
# Phillip J. Curtiss, Associate Professor
# pcurtiss@mtech.edu, 406-496-4807
# Department of Computer Science, Montana Tech
##

##
# Boot loaded must operate in 16-bit real mode
# first assembly instruction must inform assembler of this
.code16

##
# Need an entry point for the linker to organize and identify
# the start of our code; convention is __start
# .global makes this refernce available to linker
.global __start

##
# Label is the start of our assembly code
__start:
    jmp __start

# the above will simply create an infinite loop

# let's solve the size of the file problem by filling-out the binary
# with zeros, leaving 2-bytes (16 bits) worth of space at the end
# for the magic number

# the . refers to the byte position in the binary file, and labels can 
# be used to refer to the byte position in the binary file where they
# occur; so (. - __start) is a mathematical expression equal to the 
# number of bytes between the current byte in the binary file - where the
# . is located, and the start of our code, identified by the __start label
#
# we then subtract that amount from the fixed value 510 to get the number
# of bytes needed to add to the binary file to padd it out to 510 bytes
# and use the .fill instruction that takes: 
# fill <count>, <size>, <value>
.fill 510 - (. - __start), 1, 0

# this file is not bootable, as it is missing its magin number
##

