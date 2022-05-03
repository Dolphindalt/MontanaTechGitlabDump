---
Renderer: GitLab Markdown
Version: 20210126.01
Term: Spring 2021 - CSCI 460 Operating Systems
---

## Project Description

The classic _Hello World!_ program implemented in x86 assembly language using the AT&T style instruction formats for the GNU assembler. 

The program is also written in the format of a _boot loader_ program, requiring the use of _16-bit Real Mode_ that fits in a boot sector - requiring the x86 magic number present as the last two (2) bytes in the boot sector binary code. 

## Project Files

The following documents the different steps of the development of the _Hello World!_ assembly program. 

- `boot-01.s`
   - Structure of a 16-bit _Real Mode_ assembly program
   - Entry point to assembly code for use by linker
   - Infinite loop construct
   - Build: `make boot-01.o`
- `boot-02.s`
   - Padding binary file to 510 bytes using `.` operator and address of the start label and the `.fill` instruction
   - Left space in binary file for _magic number_ `0x55aa`
   - Build: `make boot-02.o`
- `boot-03.s`
   - Adding the _magic number_ to the boot loader
   - Use the `.word` instruction with magic number bytes swapped to accommodate x86 _little endian_ architecture word format
   - Build: `make boot-03.o` <br> `make boot-03.bin`
   - Execute: `qemu-system-x86_64 boot-03.bin`
- `boot-04.s`
   - Making use of `INT 0x10` BIOS routine to output to the video terminal 
   - Use register `AX` to specify the specific video function - using `0x0E` in the `AH` (high 8-bits of `AX`), and the char to output in the `AL` (low 8-bits of `AX`) to output a `char` to the terminal 
   - Build: `make boot-04.o` <br> `make boot-04.bin`
   - Execute: `qemu-system-x86_64 boot-04.bin`
- `boot-05.s`
   - Storing a string with a label serving as the address of the first byte of the string
   - Difference between `.ascii` and `.asciz` string types
      - `.asciz` allocates storage for the string with an appended null byte - `\0`
   - Using the label for the string as the _address_ of the first byte of the string
   - Moving the string _address_ into the `BX` register - the entire register since addresses are 16-bits
   - Using a `mov` instruction to copy the contents at the memory address at the address held in the `BX` register into the `AL` register (lower 8-bits of `AX`)
   - Invoking the `INT 0x10` BIOS routine to display the character on the output terminal
   - Execute the `hlt` - halt instruction
   - Build: `make boot-05.o` <br> `make boot-05.bin`
   - Execute: `qemu-system-x86_64 boot-05.bin`
- `boot-06.s`
   - Implement a looping construct using `jmp` instruction and a label
   - Within the loop, 
      - increment the _address_ in the `BX` register by `$1` to refer to the next byte in the string
      - load the value at the memory address into the `AL` register
      - compare the value in the `AL` register to the null byte `$0`
      - execute the `je` jump if equal instruction to end the loop
      - otherwise, output the character by invoking the `INT 0x10` instruction
   - Build: `make boot-06.o` <br> `make boot-06.bin`
   - Execute: `qemu-system-x86_64 boot-06.bin`
- `boot-07.s`
   - make use of the special register `SI` for string index
   - use the `lodsb` to load the next byte of the string into the `AL` register and increment the address by 1 byte in the `SI` register
   - simplifies the looping construct and does not require a separate register for counting using the address of the string index
   - Build: `make boot-07.s` <br> `make boot-07.bin`
   - Execute: `qemu-system-x86_64 boot-07.bin`
- `boot-99.s`
   - Removed much of the explanatory comments, leaving only instructions and labels
   - Build: `make boot-99.s` <br> `make boot-99.bin`
   - Execute: `qemu-system-x86_64 boot-99.bin`

## Makefile

Students should review the _build rules_ found in the `Makefile` to understand the options provided to the _loader_ and what each of those options do and why they are needed.
