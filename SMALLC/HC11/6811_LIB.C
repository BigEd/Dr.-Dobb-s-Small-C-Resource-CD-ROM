/****************************************/
/*  SMALL C COMPILER                    */
/*  SYSTEM LIBRARY FOR THE 68HC11       */
/****************************************/

#asm
ARG1  EQU   6
ARG2  EQU   8
#endasm

/* NOTE: CCARGC MUST BE PASSED THE NUMBER OF BYTES OF LOCAL (STACK) */
/*       STORAGE FROM THE CALLING ROUTINE                           */
/* EX:   sort_lst(argc)int *argc;{                                  */
/*        char maxlen;                                              */
/*        char day;                                                 */
/*        char numargs;                                             */
/*        int  year;                                                */
/*  numargs=CCARGC(1+1+1+2); /*note the param=number of local bytes */

CCARGC(n)
int n;
{
#asm
      PSHX
      PULA
      PULB
#endasm
}

/* Enable interrupts */
e_int()
{
#asm
      CLI
#endasm
}

/* Disable interrupts */
d_int()
{
#asm
      SEI
#endasm
}

/* Called with 1's in the position to be set */
bit_set(addr,val)
int addr;
char val;
{
#asm
      LDX  ARG2,Y
      LDAB ARG1+1,Y
      ORAB 0,X
      STAB 0,X
#endasm
}

/* Called with 1's in the position to be cleared */
bit_clr(addr,val)
int addr;
char val;
{
#asm
      LDX  ARG2,Y
      LDAB ARG1+1,Y
      COMB
      ANDB 0,X
      STAB 0,X
#endasm
}

pokeb(addr,val)
unsigned int addr;
unsigned char val;
{
#asm
      LDX  ARG2,Y
      LDAB ARG1+1,Y
      STAB 0,X
#endasm
}

poke(addr,val)
unsigned int addr;
int val;
{
#asm
      LDX ARG2,Y
      LDD ARG1,Y
      STD 0,X
#endasm
}

peekb(addr)
unsigned int addr;
{
#asm
      LDX  ARG1,Y
      CLRA
      LDAB 0,X
#endasm
}

peek(addr)
unsigned int addr;
{
#asm
      LDX ARG1,Y
      LDD 0,X
#endasm
}

#asm
*************************************************
***    START OF LOW LEVEL LIBRARY ROUTINES   ****
***    PARTS BORROWED FROM MOTOROLA, OTHERS  ****
***    RE-WRITTEN BY MATT TAYLOR             ****
*************************************************

********
* MUL12
__MUL12
      PSHX
      PSHX
      PSHB
      PSHA
      TSX
      LDAA 3,X
      MUL
      STD  4,X
      LDD  1,X
      MUL
      ADDB 4,X
      STAB 4,X
      LDAB 3,X
      MUL
      ADDB 4,X
      STAB 4,X
      PULX
      PULX
      PULA
      PULB
      RTS

********
* ASR12
__ASR12
      XGDX
__ASRLOOP
      CPX  #0
      BEQ  __ASRDONE
      ASRA
      RORB
      DEX
      BRA  __ASRLOOP
__ASRDONE
      RTS

********
* ASL12
__ASL12
      XGDX
__ASLLOOP
      CPX  #0
      BEQ  __ASLDONE
      ASLD
      DEX
      BRA  __ASLLOOP
__ASLDONE
      RTS

********
*  __GE
__GE
      PSHX
      CPD  0,X
      BLT  __TRUE
      BRA  __FALSE

********
* __UGE
__UGE
      PSHX
      TSX
      CPD  0,X
      BLO  __TRUE
      BRA  __FALSE

********
* __GT
__GT
      PSHX
      TSX
      CPD  0,X
      BLE  __TRUE
      BRA  __FALSE

********
* __UGT
__UGT
      PSHX
      TSX
      CPD  0,X
      BLS  __TRUE
      BRA  __FALSE

********
* __LE
__LE
      PSHX
      TSX
      CPD  0,X
      BGT  __TRUE
      BRA  __FALSE

********
* __ULE
__ULE
      PSHX
      TSX
      CPD  0,X
      BHI  __TRUE
      BRA  __FALSE

********
* __LNEG
__LNEG
      PSHX
      CPD  #0
      BNE  __FALSE
      BRA  __TRUE

********
* __LT
__LT
      PSHX
      TSX
      CPD  0,X
      BGE  __TRUE
      BRA  __FALSE

********
* __ULT
__ULT
      PSHX
      TSX
      CPD  0,X
      BHS  __TRUE
      BRA  __FALSE

********
* __EQ
__EQ
      PSHX
      TSX
      CPD  0,X
      BEQ  __TRUE
      BRA  __FALSE

********
* __NE
__NE
      PSHX
      TSX
      CPD  0,X
      BNE  __TRUE
      BRA  __FALSE

********
*__TRUE
__TRUE
      PULX
      LDD  #1
      RTS

********
*__FALSE
__FALSE
      PULX
      LDD  #0
      RTS

********
* __SWITCH

__SWITCH
      PULX
__XLOOP
      PSHX
      LDX  0,X
      CPX  #0
      PULX
      BEQ  __XDEFAULT
      CPD  2,X
      BEQ  __XMATCH
      INX
      INX
      INX
      INX
      BRA  __XLOOP
__XDEFAULT
      INX
      INX
      JMP  0,X
__XMATCH
      LDX  0,X
      JMP  0,X
#endasm
