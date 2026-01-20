#asm
        .command -ai    ; output Intel hex format

; MEMORY MAP ====================================
        .EQU RAM,H'8000     ; starting address of data RAM, NEVER 0!
        .EQU STACKTOP,H'FFD0; initial stack pointer, MUST BE EVEN!
        .EQU ROM,H'0        ; starting address of program ROM, usually 0
        .EQU LITROM,H'6000  ; ROM address where literals will be stored

        ; Notes: all code and data pointers must be nonzero.
        ; To ensure this, the RAM equate MUST BE NONZERO.  The ROM equate
        ; can be zero; the reset code will ensure nonzero code pointers.
        ; Also, the external stack must be word-aligned, so the
        ; STACKTOP equate MUST be even.

        .SEGMENT .DATA,LITROM-RAM  ; offset literals in output file
        .DATA
        .ORG RAM
        .CODE
        .ORG ROM

; IMPORTANT 8051 EQUATES ========================
        ; Bank 0 registers accessed Directly
        .EQU DR0,0
        .EQU DR1,1
        .EQU DR2,2
        .EQU DR3,3
        .EQU DR4,4
        .EQU DR5,5
        .EQU DR6,6
        .EQU DR7,7

; 8051 RESET AND INTERRUPT VECTORS ==============
        LJMP RESET
        LJMP ..IE0
..IE0:  RETI
        .RS 4
        LJMP ..TF0
..TF0:  RETI
        .RS 4
        LJMP ..IE1
..IE1:  RETI
        .RS 4
        LJMP ..TF1
..TF1:  RETI
        .RS 4
        LJMP ..RITI
..RITI: RETI
        .RS 4
        LJMP ..TF2
..TF2:  RETI

; 8051 INITIALIZATION CODE ======================
RESET:  MOV IE,#0       ; disable all irpts
        MOV PCON,#0     ; T1 baudrate not doubled
        MOV TMOD,#H'20  ; T1 mode 2, T0 mode 0
        MOV TH1,#H'FD   ; 9600 baud @ 11.0592 MHz
        SETB TCON.6     ; enable timer 1
        MOV SCON,#H'52  ; UART mode 1 (8-bit)

        ; initialize RAM data from ROM table (LITROM)
        MOV R0,#(RAM & H'FF)    ; R4:R0 points to init'zd RAM
        MOV R4,#(RAM >> 8)      
        MOV DPTR,#LITROM        ; DPTR points to LITROM area
RAMINIT:
        CJNE R0,#(ENDLITS & H'FF),DOINIT   ; compare to end
        CJNE R4,#(ENDLITS >> 8),DOINIT
        SJMP DONEINIT
DOINIT: CLR A                   ; get byte from ROM
        MOVC A,@A+DPTR
        INC DPTR
        MOV P2,R4               ; put byte in RAM
        MOVX @R0,A
        INC R0
        CJNE R0,#0,*+4
        INC R4
        SJMP RAMINIT
DONEINIT:
        MOV SP,#8               ; set hardware stack pointer
        MOV R5,#(STACKTOP >> 8) ; set small-C stack pointer
        MOV R1,#(STACKTOP & H'FF)
        MOV DPTR,#..MEMPTR      ; set MEMPTR to start of free RAM
        MOV A,#(ENDLITS & H'FF)
        MOVX @DPTR,A
        INC DPTR
        MOV A,#(ENDLITS >> 8)
        MOVX @DPTR,A
        LCALL .MAIN             ; enter main routine
        SJMP *                  ; halt here if .main returns

; SUPPORT SUBROUTINES ===========================
; get argument count in pr
.ccargc: MOV R0,B
        MOV R4,#0
        RET

; arith shift left sr by pr into pr.  ZAPS sr!
..ASL12: MOV A,DPL
        ADD A,DPL
        MOV DPL,A
        MOV A,DPH
        RLC A
        MOV DPH,A
        DJNZ R0,..ASL12
        MOV R4,DPH
        MOV R0,DPL
        RET

; arith shift right sr by pr into pr.  ZAPS sr!
..ASR12: MOV A,DPH
        MOV C,ACC.7
        RRC A
        MOV DPH,A
        MOV A,DPL
        RRC A
        MOV DPL,A
        DJNZ R0,..ASR12
        MOV R4,DPH
        MOV R0,DPL
        RET

; multiply pr by sr signed.  16x16->16
..MUL12:
        CLR PSW.5       ; PSW Flag 0 is result sign flag
        MOV A,R4        ; if primary register negative,
        JNB ACC.7,..M12a
          CPL PSW.5         ; toggle flag
          XRL DR4,#H'FF     ; negate primary register
          XRL DR0,#H'FF
          INC R0
          CJNE R0,#0,*+4
          INC R4
..M12a: MOV A,DPH       ; if secondary register negative,
        JNB ACC.7,..M12b
          CPL PSW.5         ; toggle flag
          XRL DPH,#H'FF     ; negate secondary register
          XRL DPL,#H'FF
          INC DPTR
..M12b: ACALL ..MUL12u
        JNB PSW.5,..M12c ; if result should be negative,
          XRL DR4,#H'FF     ; negate primary register
          XRL DR0,#H'FF
          INC R0
          CJNE R0,#0,*+4
          INC R4
..M12c: RET

; multiply pr by sr unsigned.  16x16->16
..MUL12u:
        MOV A,R0        ; prL*srL -> B:A -> r3:r0
        MOV B,DPL
        MUL AB
        MOV R3,B
        XCH A,R0
        MOV B,DPH       ; prL*srH -> B:A
        MUL AB          ;       add into r3
        ADD A,R3
        MOV R3,A
        MOV A,R4        ; prH*srL -> B:A
        MOV B,DPL       ;       add into r3
        MUL AB
        ADD A,R3
        MOV R4,A        ;       result->r4
        RET             ; final result in r4:r0

; divide pr by sr unsigned.  16/16->16
; quotient in r4:r0 (pr)  remainder in r7:r3
..DIV12:
        PUSH DR4        ; we'll need sign of pr later
        CLR PSW.5       ; PSW Flag 0 is result sign flag
        MOV A,R4        ; if primary register negative,
        JNB ACC.7,..D12a
          CPL PSW.5         ; toggle flag
          XRL DR4,#H'FF     ; negate primary register
          XRL DR0,#H'FF
          INC R0
          CJNE R0,#0,*+4
          INC R4
..D12a: MOV A,DPH       ; if secondary register negative,
        JNB ACC.7,..D12b
          CPL PSW.5         ; toggle flag
          XRL DPH,#H'FF     ; negate secondary register
          XRL DPL,#H'FF
          INC DPTR
..D12b: ACALL ..DIV12u
        JNB PSW.5,..D12c ; if result should be negative,
          XRL DR4,#H'FF     ; negate primary register
          XRL DR0,#H'FF
          INC R0
          CJNE R0,#0,*+4
          INC R4
..D12c: POP ACC          ; if dividend (pr) was negative,
        JNB ACC.7,..D12d
          XRL DR7,#H'FF     ; negate R7:R3 (remainder)
          XRL DR3,#H'FF
          INC R3
          CJNE R3,#0,*+4
          INC R7
..D12d: RET

; divide pr by sr unsigned.  16/16->16
; quotient in r4:r0 (pr)  remainder in r7:r3
..DIV12u:   ; DPH:DPL = divisor, remains unchanged
            ; r7:r3:r4:r0 = dividend
        MOV R7,#0
        MOV R3,#0
        MOV B,#17      ; loop counter
        CLR C
        SJMP ..DIV2
..DIV1: ; division loop
        MOV A,R3        ; shift hi 16 bits left
        RLC A
        MOV R3,A
        MOV A,R7
        RLC A
        MOV R7,A
        CLR C           ; subtract divisor from hi 16
        MOV A,R3
        SUBB A,DPL
        MOV R3,A
        MOV A,R7
        SUBB A,DPH
        MOV R7,A
        JNC ..DIV4
        MOV A,R3        ; borrow occurred -- undo the subtract
        ADD A,DPL
        MOV R3,A
        MOV A,R7
        ADDC A,DPH
        MOV R7,A
        SETB C
..DIV4: ; here cy=0 if subtracted, cy=1 if not
        CPL C
..DIV2: MOV A,R0        ; shift low 16 bits left
        RLC A
        MOV R0,A
        MOV A,R4
        RLC A
        MOV R4,A
        DJNZ B,..DIV1
        ; here r7:r3 = remainder, r4:r0 = quotient
        RET

; switch routine, argument in pr.  ZAPS sr!
; usage:        LCALL ..SWITCH
;               code adrs       \ case
;               test value      /  1
;               code adrs       \ case
;               test value      /  2
;                  ...            etc.
;               code adrs = 0 marks end of table
;               machine code (default action)
;
..SWITCH: POP DPH       ; get table address in DPTR
        POP DPL
..SWLOOP: CLR A
        MOVC A,@A+DPTR  ; get code address in r7:r3
        INC DPTR
        MOV R3,A
        CLR A
        MOVC A,@A+DPTR
        INC DPTR
        MOV R7,A
        ORL A,R3        ; IF code address = 0,
        JNZ *+3         ;    A=0, DPTR=adrs of default code,
        JMP @A+DPTR     ;    so go to default code.
        CLR A
        MOVC A,@A+DPTR  ; get test value & compare to pr
        INC DPTR
        XRL A,R0
        MOV B,A
        CLR A
        MOVC A,@A+DPTR
        INC DPTR
        XRL A,R4
        ORL A,B         ; IF test value = pr,
        JNZ ..SWLOOP    ;    A=0,
        MOV DPH,R7      ;    get code adrs in DPTR,
        MOV DPL,R3
        JMP @A+DPTR     ;    and go to that code.

; comparison routines.  test sr:pr,
; return true (1) or false (0) in pr.

..LT12u: CLR C
        MOV A,DPL
        SUBB A,R0
        MOV A,DPH
        SUBB A,R4
        JC ..TRUE
        SJMP ..FALSE

..GE12u: CLR C
        MOV A,DPL
        SUBB A,R0
        MOV A,DPH
        SUBB A,R4
        JNC ..TRUE
        SJMP ..FALSE

..GT12u: CLR C          ; sr>pr iff pr<sr!
        MOV A,R0
        SUBB A,DPL
        MOV A,R4
        SUBB A,DPH
        JC ..TRUE
        SJMP ..FALSE

..LE12u: CLR C          ; sr<=pr iff pr>=sr!
        MOV A,R0
        SUBB A,DPL
        MOV A,R4
        SUBB A,DPH
        JNC ..TRUE
        SJMP ..FALSE

..EQ12: MOV A,DPL
        XRL A,R0
        MOV R3,A
        MOV A,DPH
        XRL A,R4
        ORL A,R3
        JZ ..TRUE
..FALSE: MOV R0,#0
        MOV R4,#0
        RET

..NE12: MOV A,DPL
        XRL A,R0
        MOV R3,A
        MOV A,DPH
        XRL A,R4
        ORL A,R3
        JZ ..FALSE
..TRUE: MOV R0,#1
        MOV R4,#0
        RET

..LT12: CLR C
        MOV A,DPL
        SUBB A,R0
        MOV A,DPH
        SUBB A,R4
        JNB PSW.2,*+4
        CPL A           ; ACC.7 set if Less Than
        JB ACC.7,..TRUE
        SJMP ..FALSE

..GE12: CLR C
        MOV A,DPL
        SUBB A,R0
        MOV A,DPH
        SUBB A,R4
        JNB PSW.2,*+4
        CPL A           ; ACC.7 set if Less Than
        JNB ACC.7,..TRUE
        SJMP ..FALSE

..GT12: CLR C           ; sr>pr iff pr<sr!
        MOV A,R0
        SUBB A,DPL
        MOV A,R4
        SUBB A,DPH
        JNB PSW.2,*+4
        CPL A           ; ACC.7 set if Less Than
        JB ACC.7,..TRUE
        SJMP ..FALSE

..LE12: CLR C           ; sr<=pr iff pr>=sr!
        MOV A,R0
        SUBB A,DPL
        MOV A,R4
        SUBB A,DPH
        JNB PSW.2,*+4
        CPL A           ; ACC.7 set if Less Than
        JNB ACC.7,..TRUE
        SJMP ..FALSE
#endasm

/* global data */
char *_memptr;      /* pointer to free memory */

