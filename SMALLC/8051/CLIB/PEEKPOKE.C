#asm
 ;
 ; PEEKPOKE.C -- assembly language PEEK and POKE routines.
 ; These are roughly 1/3 the length of the equivalent
 ; routines written in Small C.
 ;
 
; PEEK(adr) -- get a 16-bit value from address
 ; peek(adr) unsigned int *adr; { *adr; }
 ;
 .PEEK:  MOV DPH,R5      ; get stack pointer in DPTR
        MOV DPL,R1
        MOVX A,@DPTR    ; get adrs in DPTR
        MOV R3,A
        INC DPTR
        MOVX A,@DPTR
        MOV DPH,A
        MOV DPL,R3
        MOVX A,@DPTR    ; fetch at adrs -> R4:R0
        MOV R0,A
        INC DPTR
        MOVX A,@DPTR
        MOV R4,A
        RET             ; return with data in primary reg
 
; POKE(adr,dta) -- poke a 16-bit value to address
 ; poke(adr,dta) unsigned int *adr,dta; { *adr=dta; }
 ;
 .POKE:  MOV DPH,R5      ; get stack pointer in DPTR
        MOV DPL,R1
        MOVX A,@DPTR    ; get data in R4:R0 (primary reg)
        MOV R0,A
        INC DPTR
        MOVX A,@DPTR
        MOV R4,A
        INC DPTR
        MOVX A,@DPTR    ; get adrs in DPTR
        MOV R3,A
        INC DPTR
        MOVX A,@DPTR
        MOV DPH,A
        MOV DPL,R3
        MOV A,R0        ; store data at adrs
        MOVX @DPTR,A
        INC DPTR
        MOV A,R4
        MOVX @DPTR,A
        RET             ; return with data in primary reg
 
; PEEKB(adr) -- get an 8-bit value from address
 ; peekb(adr) unsigned char *adr; { *adr; }
 ;
 .PEEKB: MOV DPH,R5      ; get stack pointer in DPTR
        MOV DPL,R1
        MOVX A,@DPTR    ; get adrs in DPTR
        MOV R3,A
        INC DPTR
        MOVX A,@DPTR
        MOV DPH,A
        MOV DPL,R3
        MOVX A,@DPTR    ; fetch byte at adrs -> R0
        MOV R0,A
        MOV R4,#0       ; clear high byte of data
        RET             ; return with data in primary reg
 
; POKEB(adr,dta) -- poke an 8-bit value to address
 ; pokeb(adr,dta) unsigned char *adr,dta; { *adr=dta; }
 .POKEB: MOV DPH,R5      ; get stack pointer in DPTR
        MOV DPL,R1
        MOVX A,@DPTR    ; get data in R0 (primary reg)
        MOV R0,A
        INC DPTR
        INC DPTR        ; (high byte of data ignored)
        MOVX A,@DPTR    ; get adrs in DPTR
        MOV R3,A
        INC DPTR
        MOVX A,@DPTR
        MOV DPH,A
        MOV DPL,R3
        MOV A,R0        ; store data byte at adrs
        MOVX @DPTR,A
        MOV R4,#0       ; return a valid unsigned char
        RET             ; return with data in primary reg
 
#endasm

