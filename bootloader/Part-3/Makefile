# 
# Makefile for Generating x86 bootloader
#
# CSCI 460 - Operating Systems
#
# Phillip J. Curtiss, Associate Professor
# pcurtiss@mtech.edu, 406-496-4807
# Department of Computer Science, Montana Tech
#
CC= /usr/bin/bcc
CFLAGS= -ansi
#
AS= /usr/bin/as86
#
LD= /usr/bin/ld86
LDFLAGS= -d
LIBS= $(CLIB)
#
CP= /bin/cp
DD= /bin/dd
#
CLIB= /usr/lib/bcc/libc.a
MTX= /opt/CSCI460/mtximage
IMAGE= mtximage
#
SRCS= bs.s bc.c
#
OBJ= $(SRCS:.s=.o) # substitute for assembly code
OBJS= $(OBJ:.c=.o) # substitute for C code
#
TARGET= bootloader
#
# Rules that don't make targets
.PHONY: clean
#
# Defult rule to make if no target specified on command line
default: $(TARGET)

# Rules to generate target
$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(CP) $(MTX) $(IMAGE)
	$(DD) if=$(TARGET) of=$(IMAGE) bs=1024 count=1 conv=notrunc
	@echo "now run: qemu -fda $(IMAGE) -no-fd-bootchk"
	@echo "(connect to the OS instance using vncviewer)"

# Rules to generate object code from assembly code
%.o: %.s
	$(AS) -o $@ $^

# Rules to generate object code from C code
%.o: %.c
	$(CC) $(CFLAGS) -c $@ $^

# Rule to clean up all intermediate build files
clean:
	$(RM) *.o $(TARGET) $(IMAGE)
