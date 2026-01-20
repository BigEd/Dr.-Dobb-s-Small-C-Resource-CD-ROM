divert(-1)
#********************************************************************
#
#M4 definitions for implementing the Retargetable Concurrent Small C
#virtual machine on the 80x86
#Copyright 1997 Andy W. K. Yuen
#
#********************************************************************
#
#RCSC pseudo instructions
#
#********************************************************************
define(`CSCEXTRN',
`extrn __eq: near
extrn __ne: near
extrn __le: near
extrn __lt: near
extrn __ge: near
extrn __gt: near
extrn __ule: near
extrn __ult: near
extrn __uge: near
extrn __ugt: near
extrn __lneg: near
extrn __switch: near')

define(`CSCTERM',
`extrn __cscinit:NEAR
MNTRSEG SEGMENT PUBLIC
dw 1a1ah
MNTRSEG ENDS
TASKSEG SEGMENT PUBLIC
dw 1a1ah
TASKSEG ENDS
INTRSEG SEGMENT PUBLIC
dw 1a1ah
INTRSEG ENDS')

define(`EXTERNAL',
`extrn $1:$2')

define(`TOSEG',
`define(`CURSEG', $1) ifelse($1, 1, 
`DATA SEGMENT PUBLIC',
`CODE SEGMENT PUBLIC
ASSUME CS:CODE, SS:DATA, DS:DATA')')

define(`ENDSEG',
`ifelse($1, 1, DATA ENDS, CODE ENDS)')

define(`DECLPUBLIC',
`ifelse(CURSEG, 2, 
`PUBLIC $1
$1:', 
`PUBLIC $1
DECLARE($1)')')

define(`DECLARE', 
`define(`LABELON', 1)
DECLAREMEM($1,')dnl

define(`REFm', 
`define(`LABELON', 1)
REFmMEM($1,')dnl

define(`SHADOW', `')
define(`MODULENAME', `')
define(`BEGINLIT', `')
define(`ENDLIT', `')
define(`BEGINDUMP', `')dnl

#********************************************************************
#
#macros used internally
#
#********************************************************************
define(`repeat', 
`ifelse(eval($2 <= 0), 1, , 
`repeat(`$1', decr($2))'
$1)')

define(`DECLAREMEM', 
`define(`LABELON', 0)
ifelse($2, _WORD_, `$1 DW ', 
$2, _WORDn, `$1 DW $3',
$2, _WORDr0, `$1 DW $3 DUP(0)',
$2, _BYTE_, `$1 DB ', 
$2, _BYTEn, `$1 DB $3',
`$1 DB $3 DUP(0)')')

define(`REFmMEM', 
`define(`LABELON', 0)
ifelse($2, _WORD_, `$1 DW ', 
$2, _WORDn, `$1 DW $3',
$2, _WORDr0, `$1 DW $3 DUP(0)',
$2, _BYTE_, `$1 DB ', 
$2, _BYTEn, `$1 DB $3',
`$1 DB $3 DUP(0)')')

#********************************************************************
#
#interrupt, task and monitor table creation macros
#
#********************************************************************
define(`INTRS', 
`
INTRSEG SEGMENT PUBLIC
dw $1, $2
INTRSEG ENDS
')

define(`TASKS', 
`
TASKSEG SEGMENT PUBLIC
dw $1, $2, $3
TASKSEG ENDS
')

define(`MNTRS', 
`
MNTRSEG SEGMENT PUBLIC
dw $1
MNTRSEG ENDS
')

#********************************************************************
#
#RCSC virtual machine instruction set
#
#********************************************************************
define(`ADD12',
`ADD AX,BX')
define(`ADD1n',
`ADD AX,$1')
define(`ADD21',
`ADD BX,AX')
define(`ADD2n',
`ADD BX,$1')
define(`ADDbpn',
`ADD BYTE PTR [BX],$1')
define(`ADDwpn',
`ADD WORD PTR [BX],$1')
define(`ADDm_',
`ADD $1')
define(`ADDSP',
`ADD SP,$1')
define(`AND12',
`AND AX,BX')
define(`ANEG1',
`NEG AX')
define(`ARGCNTn',
`ifelse($1, 0, `XOR CL,CL', `MOV CL,$1')')
define(`ASL12',
`MOV CX,AX
MOV AX,BX
SAL AX,CL')
define(`ASR12',
`MOV CX,AX
MOV AX,BX
SAR AX,CL')
define(`CALL1',
`CALL AX')
define(`CALLm',
`CALL $1')
define(`BYTE_', 
`ifelse(LABELON, 1, `_BYTE_, $1)', ` DB ')')
define(`BYTEn', 
`ifelse(LABELON, 1, `_BYTEn, $1)', `DB $1')')
define(`BYTEr0', 
`ifelse(LABELON, 1, `_BYTEr0, $1)', `DB $1 DUP(0)')')
define(`COM1',
`NOT AX')
define(`COMMAn',
`,$1')
define(`DBL1',
`SHL AX,1')
define(`DBL2',
`SHL BX,1')
define(`DECbp',
`DEC BYTE PTR [BX]')
define(`DECwp',
`DEC WORD PTR [BX]')
define(`DIV12',
`CWD
IDIV BX')
define(`DIV12u',
`XOR DX,DX
DIV BX')
define(`ENTER',
`PUSH BP
MOV BP,SP')
define(`EQ10f',
`OR AX,AX
JE $+5
JMP $1')
define(`EQ12',
`CALL __eq')
define(`GE10f',
`OR AX,AX
JGE $+5
JMP $1')
define(`GE12',
`CALL __ge')
define(`GE12u',
`CALL __uge')
define(`GETb1m',
`MOV AL,$1
CBW')
define(`GETb1mu',
`MOV AL,$1
XOR AH,AH')
define(`GETb1p',
`MOV AL,ifelse($1, 0, , $1)[BX]
CBW')
define(`GETb1pu',
`MOV AL,ifelse($1, 0, , $1)[BX]
XOR AH,AH')
define(`GETb1s',
`MOV AL,ifelse($1, 0, , $1)[BP]
CBW')
define(`GETb1su',
`MOV AL,ifelse($1, 0, , $1)[BP]
XOR AH,AH')
define(`GETw1m',
`MOV AX,$1')
define(`GETw1m_',
`MOV AX,')
define(`GETw1n',
`ifelse($1, 0, `XOR AX,AX', `MOV AX,$1')')
define(`GETw1p',
`MOV AX,ifelse($1, 0, , $1)[BX]')
define(`GETw1s',
`MOV AX,ifelse($1, 0, , $1)[BP]')
define(`GETw2m',
`MOV BX,$1')
define(`GETw2n',
`ifelse($1, 0, `XOR BX,BX', `MOV BX,$1')')
define(`GETw2p',
`MOV BX,ifelse($1, 0, , $1)[BX]')
define(`GETw2s',
`MOV BX,ifelse($1, 0, , $1)[BP]')
define(`GT10f',
`OR AX,AX
JG $+5
JMP $1')
define(`GT12',
`CALL __gt')
define(`GT12u',
`CALL __ugt')
define(`INCbp',
`INC BYTE PTR [BX]')
define(`INCwp',
`INC WORD PTR [BX]')
define(`WORD_', 
`ifelse(LABELON, 1, `_WORD_)', ` DW ')')
define(`WORDn', 
`ifelse(LABELON, 1, `_WORDn, $1)', `DW $1')')
define(`WORDr0', 
`ifelse(LABELON, 1, `_WORDr0, $1)', `DW $1 DUP(0)')')
define(`JMPm',
`JMP $1')
define(`LABm',
`$1')
define(`LE10f',
`OR AX,AX
JLE $+5
JMP $1')
define(`LE12',
`CALL __le')
define(`LE12u',
`CALL __ule')
define(`LNEG1',
`CALL __lneg')
define(`LT10f',
`OR AX,AX
JL $+5
JMP $1')
define(`LT12',
`CALL __lt')
define(`LT12u',
`CALL __ult')
define(`MOD12',
`CWD
IDIV BX
MOV AX,DX')
define(`MOD12u',
`XOR DX,DX
DIV BX
MOV AX,DX')
define(`MOVE21',
`MOV BX,AX')
define(`MUL12',
`IMUL BX')
define(`MUL12u',
`MUL BX')
define(`NE10f',
`OR AX,AX
JNE $+5
JMP $1')
define(`NE12',
`CALL __ne')
define(`NEARm',
` DW $1')
define(`OR12',
`OR AX,BX')
define(`PLUSn',
`$1')
define(`POINT1l',
`MOV AX,OFFSET $1')
define(`POINT1m',
`MOV AX,OFFSET $1')
define(`POINT1s',
`LEA AX,ifelse($1, 0, , $1)[BP]')
define(`POINT2m',
`MOV BX,OFFSET $1')
define(`POINT2m_',
`MOV BX,OFFSET $1')
define(`POINT2s',
`LEA BX,ifelse($1, 0, , $1)[BP]')
define(`POP2',
`POP BX')
define(`PUSH1',
`PUSH AX')
define(`PUSH2',
`PUSH BX')
define(`PUSHm',
`PUSH $1')
define(`PUSHp',
`PUSH ifelse($1, 0, , $1)[BX]')
define(`PUSHs',
`PUSH ifelse($1, 0, , $1)[BP]')
define(`PUT_m_',
`MOV $1')
define(`PUTbm1',
`MOV $1,AL')
define(`PUTbp1',
`MOV [BX],AL')
define(`PUTwm1',
`MOV $1,AX')
define(`PUTwp1',
`MOV [BX],AX')
define(`rDEC1',
`repeat(`DEC AX', $1)')
define(`rDEC2',
`repeat(`DEC BX', $1)')
define(`RETURN',
`ifelse($1, 0, ,`MOV SP,BP')
POP BP
RET')
define(`rINC1',
`repeat(`INC AX', $1)')
define(`rINC2',
`repeat(`INC BX', $1)')
define(`SUB_m_',
`SUB $1')
define(`SUB12',
`SUB AX,BX')
define(`SUB1n',
`SUB AX,$1')
define(`SUBbpn',
`SUB BYTE PTR [BX],$1')
define(`SUBwpn',
`SUB WORD PTR [BX],$1')
define(`SWAP12',
`XCHG AX,BX')
define(`SWAP1s',
`POP BX
XCHG AX,BX
PUSH BX')
define(`SWITCH',
`CALL __switch')
define(`XOR12',
`XOR AX,BX')
define(`CSCINIT',
`CALL __cscinit')
define(`ENTERM',
`CLI')
define(`EXITM',
`STI')

divert
