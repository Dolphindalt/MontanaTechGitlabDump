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
# this file is not bootable, as it is missing its magin number and 
# does not fill-out the boot sector of the device
##

