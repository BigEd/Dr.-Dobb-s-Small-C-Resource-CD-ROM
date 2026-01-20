
                    SMALL C/51 APPLICATION NOTE # 1
                              May 18, 1995
       Compiling C programs for download to monitor or Max-Forth
 
As distributed, the Small C/51 compiler will produce programs to be
 burned into EPROM.  It assumes 32K of Program PROM at address 0000, and
 32K of Data RAM at address 8000h.  The RAM stack grows downward from the
 end of memory.  ROM literals are stored beginning at address 6000h.
 This is controlled by the following lines in clib\common51.c:
 
; MEMORY MAP for PROM ====================================
        .EQU RAM,H'8000     ; starting address of data RAM, NEVER 0!
        .EQU STACKTOP,H'0   ; initial stack pointer, MUST BE EVEN!
        .EQU ROM,H'0        ; starting address of program ROM, usually 0
        .EQU LITROM,H'6000  ; ROM address where literals will be stored
 
                                 * * *
 
You can also download C programs through a serial port, using Max-Forth
 or the NMI 8051 Monitor.  In this case you MUST jumper the NMIX-0016
 memory map to put RAM U3 in both Program and Data space (so that the
 monitor can store the downloaded program in RAM):
 
    jumper J4:  U2 = program only
    jumper J5:  U2 = 32K program
    jumper J31: U3 = 32K program + data
 
Using 32K memory devices, this will produce the following memory map:
 
    U2, PROM, Program Only, 0000-7FFF hex (Monitor or Max-Forth ROM)
    U3, RAM,  Program/Data, 8000-FFFF hex
    U4, RAM,  Data Only,    0000-7FFF hex
 
To compile Small C programs for this memory map, change the first few
 lines of file clib\common51.c as follows:
 
; MEMORY MAP for download ================================
        .EQU RAM,H'0001     ; starting address of data RAM, NEVER 0!
        .EQU STACKTOP,H'8000  ; initial stack pointer, MUST BE EVEN!
        .EQU ROM,H'8000     ; starting address of program ROM, usually 0
        .EQU LITROM,H'E000  ; ROM address where literals will be stored
 
I.e., 32K of Program RAM at address 8000h, and 32K of Data RAM at
 address 0000h.  (Note that the first byte of RAM is skipped; this is a
 requirement of Small C, to ensure that no valid pointer is ever zero.)
 The stack grows downward from the end of RAM (8000h).  Literals are
 stored beginning at address E000h.
 
If you are downloading with Max-Forth, be aware that both Max-Forth and
 Small C use the 80535's internal RAM.  Small C's subroutine return stack
 grows upward from internal RAM address 8.  Thus, your Small C program
 will corrupt several crucial Max-Forth pointers and vectors.  The only
 way to return to Max-Forth is by resetting the CPU.
 
