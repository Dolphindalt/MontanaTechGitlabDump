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

# Let's make this assembly do something slightly more useful, like output
# a character on the screen by using a bios routine

# We first need to understand the different bios routines available, and 
# how each of these interrupts makes use of the different x86 architecture
# registers - we are in luck! We can leverage RBIL - Ralf Brown's IL
# https://cs.cmu.edu/~ralf/files.html
# the wiki.osdev.org/BIOS uses these interrupts to explain how best to 
# use them in 16-bit Real Mode

# from osdev, we can see Video Display Functions are available through 
# interrupt 0x10 - INT 0x10 
# Further, it is clear that the single BIOS interrupt routine is able to 
# provide a number of different functions. For our purposes, we will 
# select function 0xE - which displays a single character. 

# to use this function of the Video Display interrupt, we need to
# use the AX register - AH (high 8 bits) must be set to 0xE, and 
# the AL (low 8 bits) must be set to the char to display, then we
# simply invoke INT 0x10

# Do not want to have to write a series of inst - one for each character
# instead, we can store a string in our data segment using a label 
# decorated with .ascii or .asciz - the later appendng a null character

# once the string is stored, we can set the address of the first character
# of the string into a register, let's select BX - the entire reg since
# we are storing a 16-bit address

# we can then set AL (lower 8-bit of AX) to first byte by deref the address
# contained in the BX register

    mov $0x0e, %ah   # 0xE is the video function desired
    mov $msg,  %bx   # address of first byte of msg into bx register
    mov (%bx), %al   # contents of BX address to AL (lower 8-bit of AX)
    int $0x10        # invoke Video Interrupt calling function in ah
    hlt              # halt the system


    msg: .asciz "Hello Wrold!" # stores string with null byte - access $msg

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

# let's now add our magic number as the last two bytes of the binary file
# usng the .word instruction
.word 0xaa55

# the magic number is actually 0x55aa, but x86 architecture is 
# little endian, so the bytes need to be swapped.
##

# The resulting .bin file may be booted via
# qemu-system-x86_64 boot-03.bin


