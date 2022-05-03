---
Renderer: GitLab Markdown
Version: 20210126.01
Term: Spring 2021 - CSCI460 Operating Systems
---

| Develop and deploy Boot Loader for MTX OS  | S21 CSCI460 Operating Systems  |
| :--- | :--- |
| Assigned: **2021-01-26** | Due Date: **See Parts** | 
| Abstract: | Provide C Language to Prompt user for Kernel, find Kernel in EXT2 Filesystem, Load Kernel into memory, and boot kernel - Boot Loading the MTX OS |
| Objectives: | 1. Understand principles of boot loading<br> 2. Structs associated with EXT2 Filesystem <br> 3. Process of Loading and OS <br> 4. Using Assembly, C, and root filesystems to load an OS|
| Grading: 45 pts| A (&ge;41.85); A- (&ge;40.50); <br> B+ (&ge;39.15); B (&ge;37.35); B- (&ge;36.00) <br> C+ (&ge;34.65); C (&ge;32.85); C- (&ge;31.75) <br> D+ (&ge;30.15); D (&ge;28.35); D- (&ge;27) |
| Outcomes: | R8 (CS: 5-1, 2, 5-a-4; SE: 1, III-1-2-1) <br> (see syllabus for description of course outcomes)|

## Project Description:

Using the files provided, create a working boot loader for the MTX operating system. This project is in four parts:

### Project  Schedule

- Part-0: (Due: **2021-02-01**) Clone the project; build the project; execute and capture output; study and understand all code provided - assembly, C-language, Makefile; ask questions
- Part-1: (Due: **2021-02-08**) Write C Code to call assembly routines, handle basic I/O from/to terminal
- Part-2: (Due: **2021-02-15**) Write the C Code using the _mailman_ algorithm to map from linear filesystem block number to cylinder, head, sector - locate and print out directory structure of kernel directory
- Part-3: (Due: **2021-02-22**) Use previous parts to load and boot the MTX OS - should be similar to booted kernel from Part-0 when completed

## Lab Environment

   The lab environment will take place on the virtual machine (VM) `csdept11.cs.mtech.edu`. You may ssh into this machine using your normal Department Linux login and password. 

   Upon logging into the VM, you will be presented your unique `port number` for connection to a VNC session when using the `qemu` emulation software used in connection with the MTX operating system. Remember this port. If you forget the port number, you may type the command `myport` to be presented your port number again. This is the port you will need to use to access your MTX operating system instance, both internally - from the Campus networks - and externally - from your home or off-campus networks. 

## Specific Project Tasks:

The following tasks should be completed to obtain the maximum points available for this assignment.

1. Obtaining your project files  
   1. Perform the following tasks on the Department GitLab Server:  
      1. Login to the Department GitLab server [gitlab.cs.mtech.edu](https://gitlab.cs.mtech.edu) with your email address and your password. 
      1. Select the project operating-systems/s21-csci460/bootloader> from the offering of this course.
      1. Click on the **fork** button to create a copy of this project under your own GitLab account.
      1. Go to the **project settings** menu and then the **members** option and add your instructor and any teaching assistants as developers to your copy of the project. 
      1. Lastly, go back to this project under your account and click the icon to the right of the project URL. This will copy the project URL to the clipboard so you may paste it in the next sequence of steps. 
   1. Perform the following tasks on the Department Linux server `csdept11.cs.mtech.edu`. 
      1. Login to the Department's Linux server with your credentials. 
      1. Execute the command(s) 
      ```
      cd ~/CSCI460/Projects
      ```
      3. Execute the command `git clone <url>`, where `<url>` is pasted from the clipboard.
      1. Change into the directory created by the `git clone` command to perform the rest of the tasks for this project. 

- Part-0: Explore the MTX Operating System by running in the qemu emulator and connecting to the instance.  
   1. Execute the following command(s):
   > cd ~/CSCI460/Projects/bootloader/Part-0  
     qemu -fda mtximage -no-fd-bootchk

   and then, on your lap workstation, launch the `vncviewer` as follows:  
   > vncviewer csdept11.cs.mtech.edu:<port>

   note, your port number will be the one presented to you upon login, or your can retrieve using the `myport` command. 

   2. You should see the MTX system `boot:` prompt. Just hit enter, and then answer `Y` to the load the OS
   2. You will see the OS load and, eventually, a login prompt. The default credentials are: 
   > Username: root  
     Password: 12345

   3. Explore the MTX OS commands. Capture the output of using some of the commands and include this in a lab report `report.pdf` located in your project's root directory.

- Part-1: Writing C Code to use the assembly routines to prompt the user for the name of the kernel to boot and displays this string back to the user.  
   1. Execute the following command(s):  
   > cd ~/CSCI460/Projects/bootloader/Part-1

   2. You have been given the assembly code file `bs.s`, a skelton C file `bc.c`, and a `Makefile` that you may use to create an MTX etx2 filesystem `mtximage`.
   2. You are to finish writing the contents of the C file `bc.c` so that the upon booting of the `mtximage`, the user is prompted for the name of the kernel to boot, and then prints the string provided back to the terminal. It will continue to do this until only a newline is provided as input, upon which the program will halt. 
   2. Please make sure you understand what is being done in the assembly code, and make sure you use proper commenting in your C program, handling cases such as the user entering a backspace key, and making sure you do not go beyond the bounds of the buffer in which input is being collected. 
   2. Make sure your boot loader - the result of compiling the `bc.c`, assembling the `bs.s` and linking these both into a binary image - does not exceed the size of the `boot block` or 1024 bytes. 
   2. Describe (briefly) how the `Makeefile` is functioning to incorporate your `bootloader` binary code into the `mtximage` file and include in your `report.pdf`.

- Part-2: Reading contents of the EXT2 filesystem from a binary image and locating the kernel and displaying its location.  
   1. Execute the following command(s):  
   > cd ~/CSCI460/Projects/bootloader/Part-2 

   2. You have been provided assembly code, `bs.s`, and a C file, `bc.c`, and an EXT2 image file, `mtximage`, and a `Makefile`. These files are to be used to explore the EXT2 filesystem and locate specific directories, display their contents, and eventually locate the kernel file within the filesystem. 
   2. The system call `readfd(cyl, head, sector, buf)` is located in the assembly and makes use of `INT 0x13` to read a disk block (2 sectors) at (cyl, head, sector) and store the data into a memory location referenced by the pointer `buf.`. 

      When reading and writing from/to disk, BIOS `INT 0x13` uses the `(segment, offset) = (ES, BX)` as the physical memory address. In `readfd(...)`, `buf` is loaded into register `BX`, so the real address is `(ES, buf)`, where `buf` is an offset within the extended segment (i.e. `ES`).

      Example:  
      > ES -> data segment of a program  
        buf is global variable in the C program  
        readfd(cyl, head, sector, buf) reads a block into buf  
        ES -> 0x2000  
        buf = 0  
        readfd(cyl, head, sector, buf) reads a block to (0x2000 << 4 + 0)

   4. Changing linear block number into corresponding CHS (cyl, head, sector) format. The filesystem uses a linear disk block format, however the BIOS `INT 0x13` requires CHS format. You can use the _mailman_ algorithm to convert from linear block address to the triple of CHS. 
   4. Print ALL the directory entries under the `/` directory (root directory). Provide the output in your `report.pdf`
   4. Print ALL the directory entries under the `/boot` directory. Provide the output in your report. 

- Part-3: Loading the MTX kernel from the EXT2 filesystem and booting the OS
   1. Execute the following command(s):  
   > cd ~/CSCI460/Projects/bootloader/Part-3

   2. You have been provided assembly code, `bs.s`, C code, `bc.c`, and EXT2 image file, `mtximage`, and a `Makefile`. These files are to be used to load the kernel from the `/boot/mtx` binary file in the `mtximage` EXT2 filesystem. You will need to locate the file within the EXT2 filesystem and then load its contents into the specific address `0x1000` agreed to by the code in the assembly file `bs.s`. 
   2. Write your `main()` function in the `bc.c` file to load the disk blocks associated with the file `/boot/mtx` into segment `0x1000`. When loading is complete, return from your C code, back to the assembly that called the `main` C function, which will execute the immediate jump instruction, `jmpi 0x,0x1000` to begin executing instructions from the MTX kernel. 

## Project Grading: 

The project must compile without error (ideally without warnings) and should not fault upon execution. All exceptions should be caught if thrown and handled in a rational manner. Grading will follow the *project grading rubric* shown below: 

| Attribute (Pts) | Exceptional (1) | Acceptable (0.8) | Amateur (0.7) | Unsatisfactory (0.6) | 
| :---:           | :---            | :---             | :---          | :---                 |
| Specification (10) | The program works and meets all of the specifications. | The program works and produces correct results and displays them correctly. It also meets most of the other specifications. | The program produces correct results, but does not display them correctly. | The program produces incorrect results. |
| Readability (10) | The code is exceptionally well organized and very easy to follow. | The code is fairly easy to read. | The code is readable only by someone who knows what it is supposed to be doing. | The code is poorly organized and very difficult to read. | 
| Reusability (10) | The code only could be reused as a whole or each routine could be reused. | Most of the code could be reused in other programs. | Some parts of the code could be reused in other programs. | The code is not organized for reusability. | 
| Documentation (10) | The documentation is well written and clearly explains what the code is accomplishing and how. | The documentation consists of embedded comments and some simple header documentation that is somewhat useful in understanding the code. | The documentation is simply comments embedded in the code with some simple header comments separating routines. | The documentation is simply comments embedded in the code and does not help the reader understand the code. | 
| Efficiency (5) | The code is extremely efficient without sacrificing readability and understanding. | The code is fairly efficient without sacrificing readability and understanding. | The code is brute force and unnecessarily long. | The cod eis huge and appears to be patched together. | 
| Delivery (total) | The program was delivered on-time. | The program was delivered within a week of the due date | The program was delivered within 2-weeks of the due date. | The code was more than 2-weeks overdue. | 

### Example: 

The *delivery* attribute weights will be applied to the total score from the other attributes. If a project scores 36 points total for the sum of *specification*, *readability*, *reusability*, *documentation*, and *efficiency* attributes, but was turned in within 2-weeks after the due date, the project score would be $`36\cdot 0.7 = 25.2`$. 

## Collaboration Opportunities: 

This project provides *no collaboration opportunities* for the students. Students are expected to design and implement an original solution specific in this project description. Any work used that is not original should be cited and properly references in both the code and in any accompanying write-up. Failure to cite code that is not original may lead to claims of academic dishonesty against the student - if in doubt of when to cite, see your instructor for clarification. 


  
