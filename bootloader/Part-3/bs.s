!
! CSCI 460 Operating Systems
!
! Assembly Part of the Boot Loader for the MTX Operating Systems
!
! Phillip J. Curtiss, Associate Professor
! pcurtiss@mtech.edu, 406-496-4807
! Department of Computer Science, Montana Tech
! ---------------------------------------------------------------------
! Adapted from K.C. Wang <kwang@eecs.wsu.edu>
! Licensed via GNU General Public License <http://www.gnu.org/Licenses>
! ---------------------------------------------------------------------

!================== bs.s file ===========================================
    BOOTSEG  =  0x7C00     ! Boot block is loaded again to here.
    OSSEG    =  0x1000     ! OS Kernel is loaded here.
    SSP      =  32*1024
    BSECTORS =  2          ! load 2 sectors initially
	
    .globl _main,_prints,_NSEC                       ! IMPORT symbols
    .globl _getc,_putc,_readfd,_setes,_inces,_error  ! EXPORT symbols
	                                                
    !-------------------------------------------------------
    ! Only one SECTOR loaded at (0000,7C00). Get entire BLOCK in
    !-------------------------------------------------------
    mov  ax,#BOOTSEG
    mov  es,ax           ! set ES to BOOTSEG=0x7C00
    xor  bx,bx           ! zero bx register

    xor  dx,dx           ! zero dx register
	xor  cx,cx           ! zero cx register
	incb cl              ! increment the 8 bit counter cl register 

	movb ah,#2           ! set (high-order) byte of the ah register
	movb al,#BSECTORS    ! set (low-order) byte of the  al register
	int  0x13            ! invoke BIOS disk read via interupt vector 13h

	jmpi next,BOOTSEG    ! intrasegment jump 
next:                    
	mov  ax,cs           ! set register ax with code segment register cs
	mov  ds,ax           ! set data segment register ds with register ax
	mov  ss,ax           ! set stack segment register ss with register ax
	mov  sp,#SSP         ! set the stack pointer sp to #SSP value

	mov  ax,#0x0012      ! set register ax with value for color mode
	int  0x10            ! invoke BIOS video via interupt vector 10h
	
    call _main           ! call main() in C code base
                         ! code should find OS Kernel
                         ! load kernel into OSSEG memory location
     
    jmpi 0,OSSEG         ! immediate jump to OSSEG
                         ! to begin execution of kernel
 

!======================== I/O functions =================================
    !---------------------------------------
    ! readfd(cyl, head, sector, buf)
    !        4     6     8      10
    !---------------------------------------
_readfd:                             
    push  bp               ! preserve stack base pointer for later return
	mov   bp,sp            ! bp = stack frame pointer

    movb  dl, #0x00        ! drive 0=FD0
    movb  dh, 6[bp]        ! head
    movb  cl, 8[bp]        ! sector
    incb  cl               ! increase 8 bit conter cl register
    movb  ch, 4[bp]        ! cyl
    mov   bx, 10[bp]       ! BX=buf ==> memory addr=(ES,BX)
    mov   ax, #0x0202      ! READ 2 sectors (block) to (EX, BX)

    int  0x13              ! invoke BIOS disk read via interupt vector 13h
                           ! EX, BX have contain the data following interupt
    jb   _error            ! to error if CarryBit is on [read failed]

    pop  bp                ! retore stack pointer before return 
	ret                    ! return to caller
		
    !---------------------------------------------
    !  char getc()   function: returns a char
    !---------------------------------------------
_getc:
    xorb   ah,ah           ! zero ah byte register
    int    0x16            ! invoke BIOS terminal read via interupt 16h
                           ! char read is stored in AX register
    ret                    ! return to caller

    !----------------------------------------------
    ! void putc(char c)  function: print a char
    !----------------------------------------------
_putc:           
    push   bp              ! preserve stack base pointer for later return
	mov    bp,sp           ! bp = stack frame pointer (SP)
	
    movb   al,4[bp]        ! get the char into al register 
    movb   ah,#14          ! aH = 14
    mov    bx,#0x000C      ! bL = cyan color 
    int    0x10            ! invoke BIOS video via interupt vector 10h

    pop    bp              ! restore stack pointer before return
	ret                    ! return to caller

    !-----------------------------------------------
    ! void setes(int addr) function: set ES register
    !-----------------------------------------------
_setes:  
    push  bp               ! preserve stack base pointer for later return 
    mov   bp,sp            ! bp = stack frame pointer (SP)

    mov   ax,4[bp]         ! set ax register with parameter 
    mov   es,ax            ! set extra segment es register with ax register

    pop   bp               ! restore stack pointer before return 
    ret                    ! return to caller 
	
    !-----------------------------------
    ! void inces() function: increase ES
    !-----------------------------------
_inces:                    ! inces() inc ES by NSEC*0x20
    mov   ax,es            ! set ax register with extra segment es register
    mov   bx,_NSEC         ! set the base bx register with a single memory segment
    shl   bx,#5            ! shift left the base bx register by 5
    add   ax,bx            ! add the additional memory segment to the extra segment es register
    mov   es,ax            ! update the extra segement es register with modified memory segment
    ret                    ! return to the caller

    !------------------------------
    !       error & reboot
    !------------------------------
_error:
    push #msg              ! push the ascii string onto the stack
    call _prints           ! call the C prints function
    int  0x19              ! invoke the BIOS reboot interupt vector 19h

msg:    .asciz  "Error"        ! Error Message to output
