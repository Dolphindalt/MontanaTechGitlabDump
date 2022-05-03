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

    mov $0x0e, %ah   # 0xE is the video function desired from INT 0x10
    mov $msg,  %si   # address of first byte of msg into si register

print_char:
    lodsb            # load string byte at address SI into AL and
                     # automatically increments address in SI by 1 byte
    cmp $0,    %al   # compare value in AL to 0
    je  done         # jump if equal to done if AL == 0
    int $0x10        # invoke Video Interrupt calling function in ah
    add $1, %bx      # increment cx to next address in msg string
    jmp print_char   # jump print the next character in msg

done:
    hlt              # halt the system

    msg: .asciz "Hello Wrold!" # stores string with null byte - access $msg

    .fill 510 - (. - __start), 1, 0 # fill out to 510 bytes

    .word 0xaa55     # add magic number at end of binary file little endian

# The resulting .bin file may be booted via
# qemu-system-x86_64 boot-03.bin


