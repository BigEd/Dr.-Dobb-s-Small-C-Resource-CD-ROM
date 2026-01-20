divert(-1)
#********************************************************************
#
#M4 definitions for implementing the Retargetable Concurrent Small C
#virtual machine on the 8051
#Copyright 1997 Andy W. K. Yuen
#
#********************************************************************
#
#register assignments
#
#********************************************************************
define(`VSPL', 19h)
define(`VSPH', 18h)
define(`VBPL', 1bh)
define(`VBPH', 1ah)
define(`VR3', 1ch)
define(`VR2', 1dh)
define(`VR1', 1eh)
define(`VR0', 1fh)
define(`CP2', 21h)
define(`CA', 22h)
define(`CB', 23h)
define(`CDPL', 25h)
define(`CDPH', 24h)
define(`V1L', `R3')
define(`V1H', `R2')
define(`V2L', `R5')
define(`V2H', `R4')
define(`VLCNT', `R6')
define(`VACNT', `R7')
define(`V1SIGN', `0')
define(`V2SIGN', `1')
define(`VRSIGN', `2')

#********************************************************************
#
#interrupt vector locations
#
#********************************************************************
define(`VECTOR_0', 03h)
define(`VECTOR_1', 0bh)
define(`VECTOR_2', 13h)
define(`VECTOR_3', 1bh)
define(`VECTOR_4', 23h)
define(`VECTOR_5', 2bh)
define(`VECTOR_6', 33h)
define(`VECTOR_7', 3bh)
define(`VECTOR_8', 43h)
define(`VECTOR_9', 4bh)
define(`VECTOR_10', 53h)
define(`VECTOR_11', 5bh)

#********************************************************************
#
#macros used internally
#
#********************************************************************
define(`ADJUST',
`ifelse(eval(index($1, `-') >= 0), 1, 
`eval(($1-STACK)+5)', 
`eval(($1*-1)+3)')')

define(`repeat', 
`ifelse(eval($2 <= 0), 1, , `repeat(`$1', decr($2))'$1)')

define(`MAKENAME', `$1_$2')

define(`OPxy',
`MOV A,$3
ifelse(len($5), 0, `$1 A', `$1 A,$5')
MOV $3,A
MOV A,$4
ifelse(len($6), 0, `$1 A', `$2 A,$6')
MOV $4,A')

define(`OPxn',
`MOV A,$3
$1 A,#$5
MOV $3,A
MOV A,$4
$2 A,#$6
MOV $4,A')

define(`OPpn',
`MOV DPL,V2L
MOV DPH,V2H
INC DPTR
MOVX A,@DPTR
$1 A,#$3
MOVX @DPTR,A
ifelse(len($2), 0,,
`MOV DPL,V2L
MOV DPH,V2H
MOVX A,@DPTR
$2 A,#$4
MOVX @DPTR,A')')

define(`COPYxy',
`MOV $1,#$3
MOV $2,#$4')

define(`COPYxm',
`MOV DPTR,#$3
MOV $1,DPL
MOV $2,DPH')

#********************************************************************
#
#RCSC pseudo instructions
#
#********************************************************************
define(`CSCEXTRN',
`
extern code __addsp
extern code __enter
extern code __getb1p
extern code __getb1s
extern code __getb1su
extern code __getw1p
extern code __getw1s
extern code __getw2p
extern code __getw2s
extern code __point1s
extern code __point2s
extern code __pushm
extern code __pushp
extern code __pushs
extern code __ge12
extern code __gt12
extern code __le12
extern code __lneg1
extern code __lt12
extern code __switch
extern code __uge12
extern code __ugt12
extern code __ule12
extern code __ult12
extern code __return
extern code __and12
extern code __asl12
extern code __asr12
extern code __call1
extern code __cscinit
extern code __div12
extern code __udiv12
extern code __eq12
extern code __mod12
extern code __umod12
extern code __mul12
extern code __umul12
extern code __ne12
extern code __pop2
extern code __push1
extern code __push2
')
define(`CSCTERM', `')
define(`EXTERNAL',
`ifelse($2, NEAR, `extern code $1', `extern xdata $1')')
define(`TOSEG',
`ifelse($1, CURSEG, , $1, 1,
`ifelse(MNAME, libc,
`SEG XDATA define(`CURSEG', 1)', 
`SEG XDATA AT MAKENAME(MNAME, var_at) define(`CURSEG', 1)')', 
`SEG CODE define(`CURSEG', 2)')')
define(`ENDSEG', `')
define(`DECLPUBLIC',
`
PUBLIC $1:
')
define(`DECLARE', `$1:')
define(`SHADOW',
`define(`SHADOWING', 0)
DS $1')
define(`MODULENAME', 
`define(`MNAME', $1)')
define(`BEGINLIT', 
` SEG CODE AT MAKENAME(MNAME,lit_at) define(`CURSEG', 2)')
define(`ENDLIT', `')
define(`BEGINDUMP', `;;not supposed to be present')

#********************************************************************
#
#interrupt, task and monitor table creation macros
#
#********************************************************************
define(`INTRS', 
`
__intr_$2:
LCALL	__handler	;;common interrupt handler
DW	$1		;;user interrupt function
DW	$2		;;interrupt number
')

define(`SETVECTOR',
`SEG CODE AT VECTOR_$2
LJMP	$1
')

define(`BEGINTASK',
`
SEG CODE AT MAKENAME(MNAME, task_at)
')

define(`ENDTASK', `')
define(`TASKS', 
`
DW	$1		;;task function
DW	$2		;;task priority
DW	$3		;;task stack size
')

define(`BEGINMON',
`
SEG CODE AT MAKENAME(MNAME,mon_at)
')

define(`ENDMON', `')
define(`MNTRS', 
`
LJMP	$1		;;monitor initialization function
')

define(`BEGININTR',
`')
define(`ENDINTR', `')

#********************************************************************
#
#RCSC virtual machine instruction set
#
#********************************************************************
define(`ADD12',
``;;ADD12'
OPxy(ADD, ADDC, V1L, V1H, V2L, V2H)')

define(`ADD1n',
``;;ADD1n($1)'
OPxn(ADD, ADDC, V1L, V1H, LOW ($1), HIGH ($1))')

define(`ADD21',
``;;ADD21'
OPxy(ADD, ADDC, V2L, V2H, V1L, V1H)')

define(`ADD2n',
``;;ADD2n($1)'
OPxn(ADD, ADDC, V2L, V2H, LOW ($1), HIGH ($1))')

define(`ADDbpn',
``;;ADDbpn($1)'
OPpn(ADD, , LOW ($1), HIGH ($1))')

define(`ADDwpn',
``;;ADDwpn($1)'
OPpn(ADD, ADDC, LOW ($1), HIGH ($1))')

define(`ADDm_',
`ADDm_COMMAn($1')

define(`ADDm_COMMAn',
``;;ADDm_COMMAn($1, $2)'
MOV DPTR,#$1+1
MOVX A,@DPTR
ADD A,# LOW($2)
MOVX @DPTR,A
MOV DPTR,#$1
MOVX A,@DPTR
ADDC A,# HIGH($2)
MOVX @DPTR,A')

define(`ADDSP',
``;;ADDSP($1)'
ifelse(eval($1 < 0), 1, `define(`STACK', $1)',)
`MOV DPTR,#' eval(`$1'*-1)
LCALL __addsp')

define(`AND12',
``;;AND12'
OPxy(ANL, ANL, V1L, V1H, V2L, V2H)')

define(`ANEG1',
``;;ANEG1'
COM1
rINC1(1)')

define(`ARGCNTn',
``;;ARGCNTn($1)'
MOV VACNT,#$1')

define(`ASL12',
``;;ASL12'
LCALL __asl12')

define(`ASR12',
``;;ASR12'
LCALL __asr12')

define(`CALL1',
``;;CALL1'
LCALL __call1')

define(`CALLm',
``;;CALLm($1)'
LCALL $1')

define(`BYTE_',
` DB ')

define(`BYTEn',
` DB $1')

define(`BYTEr0',
`Db 0 repeat(``,'0', decr($1))')

define(`COM1',
``;;COM1'
OPxy(CPL, CPL, V1L, V1H)')

define(`COMMAn',
`,$1)')

define(`DBL1',
``;;DBL1'
CLR C
OPxy(RLC, RLC, V1L, V1H)')

define(`DBL2',
``;;DBL2'
CLR C
OPxy(RLC, RLC, V2L, V2H)')

define(`DECbp',
``;;DECbp'
CLR C
OPpn(SUBB, , 1)')

define(`DECwp',
``;;DECwp'
CLR C
OPpn(SUBB, SUBB, 1, 0)')

define(`DIV12',
``;;DIV12'
LCALL __div12')

define(`DIV12u',
``;;DIV12u'
LCALL __udiv12');

define(`ENTER',
``;;ENTER'
POP B
POP ACC
LCALL __enter
')

define(`EQ10f',
``;;EQ10f'
MOV A,V1L
ORL A,V1H
JZ $+5
LJMP $1')

define(`EQ12',
``;;EQ12'
LCALL __eq12')

define(`GE10f',
``;;GE10f'
MOV A,V1H
JNB ACC.7,$6
LJMP $1')

define(`GE12',
``;;GE12'
LCALL __ge12')

define(`GE12u',
``;;GE12u'
LCALL __uge12')

define(`GETb1m',
``;;GETb1m($1)'
MOV DPTR,#$1
MOVX A,@DPTR
MOV V1L,A
RLC A
MOV A,#255
JC $+3
CLR A
MOV V1H,A')

define(`GETb1mu',
``;;GETb1mu($1)'
MOV DPTR,#$1
MOVX A,@DPTR
MOV V1L,A
MOV V1H,#0')

define(`GETb1p',
``;;GETb1p($1)'
MOV DPTR,# $1
LCALL __getb1p')

define(`GETb1pu',
``;;GETb1pu($1)'
MOV DPTR,# $1
LCALL __getb1p')

define(`GETb1s',
``;;GETb1s($1)'
ifdef(1, 1, `MOV DPTR,#' ADJUST($1))
ifelse(eval(ADJUST($1) < 0), 1, `INC DPTR',)
LCALL __getb1s');

define(`GETb1su',
``;;GETb1su($1)'
ifdef(1, 1, `MOV DPTR,#' ADJUST($1))
LCALL __getb1su')

define(`GETw1m',
``;;GETw1m($1)'
MOV DPTR,#$1
MOVX A,@DPTR
MOV V1H,A
INC DPTR
MOVX A,@DPTR
MOV V1L,A')

define(`GETw1m_',
`GETw1m_PLUSn($1')

define(`GETw1m_PLUSn',
``;;GETw1m_PLUSn($1)'
MOV DPTR,#$1
MOVX A,@DPTR
MOV V1L,A
INC DPTR
MOVX A,@DPTR
MOV V1H,A')

define(`GETw1n',
``;;GETw1n($1)'
COPYxy(V1L, V1H, LOW ($1), HIGH ($1))')

define(`GETw1p',
``;;GETw1p($1)'
MOV DPTR,# $1
LCALL __getw1p')

define(`GETw1s',
``;;GETw1s($1)'
ifdef(1, 1, `MOV DPTR,#' ADJUST($1))
LCALL __getw1s')

define(`GETw2m',
``;;GETw2m($1)'
MOV DPTR,#$1
MOVX A,@DPTR
MOV V2H,A
INC DPTR
MOVX A,@DPTR
MOV V2L,A')

define(`GETw2n',
``;;GETw2n($1)'
COPYxy(V2L, V2H, LOW ($1), HIGH ($1))')

define(`GETw2p',
``;;GETw2p($1)'
MOV DPTR,# $1
LCALL __getw2p')

define(`GETw2s',
``;;GETw2s($1)'
ifdef(1, 1, `MOV DPTR,#' ADJUST($1))
LCALL __getw2s')

define(`GT10f',
``;;GT10f'
MOV A,V1H
JB ACC.7,$+7
ORL A,V1L
JNZ $+5
LJMP $1')

define(`GT12',
``;;GT12'
LCALL __gt12')

define(`GT12u',
``;;GT12u'
LCALL __ugt12')

define(`INCbp',
``;;INCbp'
OPpn(ADD, , 1)')

define(`INCwp',
``;;INCwp'
OPpn(ADD, ADDC, 1, 0)')

define(`WORD_',
` DW ')

define(`WORDn',
` DW $1')

define(`WORDr0',
`ifelse(CURSEG, 2, `DW 0 repeat(``,'0', decr($1))',
`RW $1')')

define(`JMPm',
``;;JMPm($1)'
LJMP $1')

define(`LABm',
`
$1')

define(`LE10f',
``;;LE10f'
MOV A,V1H
JNB ACC.7,$+7
ORL A,V1L
JZ $+5
LJMP $1')

define(`LE12',
``;;LE12'
LCALL __le12')

define(`LE12u',
``;;LE12u'
LCALL __ule12')

define(`LNEG1',
``;;LNEG1'
LCALL __lneg1')

define(`LT10f',
``;;LT10f'
MOV A,V1H
JB ACC.7,$+6
LJMP $1')

define(`LT12',
``;;LT12'
LCALL __lt12')

define(`LT12u',
``;;LT12u'
LCALL __ult12')

define(`MOD12',
``;;MOD12'
LCALL __mod12')

define(`MOD12u',
``;;MOD12u'
LCALL __umod12')

define(`MOVE21',
``;;MOVE21'
MOV A,V1L
MOV V2L,A
MOV A,V1H
MOV V2H,A')

define(`MUL12',
``;;NUL12'
LCALL __mul12')

define(`MUL12u',
``;;NUL12u'
LCALL __umul12')

define(`NE10f',
``;;NE10f'
MOV A,V1L
ORL A,V1H
JNZ $+5
LJMP $1')

define(`NE12',
``;;NE12'
LCALL __ne12')

define(`NEARm',
` DW $1')

define(`OR12',
``;;OR12'
OPxy(ORL, ORL, V1L, V1H, V2L, V2H)')

define(`PLUSn',
`+ $1')

define(`POINT1l',
``;;POINT1l($1)'
MOV DPTR, # $1
MOV V1L,DPL
MOV V1H,DPH')

define(`POINT1m',
``;;POINT1m($1)'
COPYxm(V1L, V1H, $1)')

define(`POINT1s',
``;;POINT1s($1)'
ifdef(1, 1, `MOV DPTR,#' ADJUST($1))
LCALL __point1s')

define(`POINT2m',
``;;POINT2m($1)'
COPYxm(V2L, V2H, $1)')

define(`POINT2m_',
`POINT2m_PLUSn($1')

define(`POINT2m_PLUSn',
``;;POINT2m_PLUSn($1)'
MOV DPTR,#$1
MOVX A,@DPTR
MOV V2H,A
INC DPTR
MOVX A,@DPTR
MOV V2L,A')

define(`POINT2s',
``;;POINT2s($1)'
ifdef(1, 1, `MOV DPTR,#' ADJUST($1))
LCALL __point2s')

define(`POP2',
``;;POP2'
LCALL __pop2')

define(`PUSH1',
``;;PUSH1'
LCALL __push1')

define(`PUSH2',
``;;PUSH2'
LCALL __push2')

define(`PUSHm',
``;;PUSHm($1)'
MOV DPTR,# $1
LCALL __pushm')

define(`PUSHp',
``;;PUSHp($1)'
MOV DPTR,# $1
LCALL __pushp')

define(`PUSHs',
``;;PUSHs($1)'
ifdef(1, 1, `MOV DPTR,#' ADJUST($1))
LCALL __pushs')

define(`PUT_m_',
`PUT_m_COMMAn($1')dnl

define(`PUT_m_COMMAn',
``;;PUT_m_COMMAn($1, $2)'
MOV DPTR,#$1
MOV A,# HIGH($2)
MOVX @DPTR,A
INC DPTR
MOV A,# LOW($2)
MOVX @DPTR,A')

define(`PUTbm1',
``;;PUTbm1($1)'
MOV DPTR,# $1
MOV A,@DPTR
MOV V1L,A
MOV V1H,#0
JNB ACC.7,$+5
MOV V1H,#-1')

define(`PUTbp1',
``;;PUTbp1'
MOV DPL,V2L
MOV DPH,V2H
MOV A,V1L
MOVX @DPTR,A')

define(`PUTwm1',
``;;PUTwm1'
MOV DPTR,#$1
MOV A,V1H
MOVX @DPTR,A
INC DPTR
MOV A,V1L
MOVX @DPTR,A')

define(`PUTwp1',
``;;PUTwp1'
MOV DPL,V2L
MOV DPH,V2H
MOV A,V1H
MOVX @DPTR,A
INC DPTR
MOV A,V1L
MOVX @DPTR,A')

define(`rDEC1',
``;;rDEC1($1)'
CLR C
OPxn(SUBB, SUBB, V1L, V1H, LOW ($1), HIGH ($1))')

define(`rDEC2',
``;;rDEC2($1)'
CLR C
OPxn(SUBB, SUBB, V2L, V2H, LOW ($1), HIGH ($1))')

define(`REFm',
`
$1:')

define(`RETURN',
``;;RETURN($1)' 
ifdef(1, 1, `MOV DPTR,#' eval($1*-1)-4)
LJMP __return
')

define(`rINC1',
``;;rINC1($1)'
OPxn(ADD, ADDC, V1L, V1H, LOW ($1), HIGH ($1))')

define(`rINC2',
``;;rINC2($1)'
OPxn(ADD, ADDC, V2L, V2H, LOW ($1), HIGH ($1))')

define(`SUB_m_',
`SUB_m_COMMAn($1')

define(`SUB_m_COMMAn',
``;;SUB_m_COMMAn($1, $2)'
MOV DPTR,#$1+1
MOVX A,@DPTR
CLR C
SUBB A,# LOW($2)
MOVX @DPTR,A
MOV DPTR,#$1
MOVX A,@DPTR
SUBB A,# HIGH($2)
MOVX @DPTR,A')

define(`SUB12',
``;;SUB12'
CLR C
OPxy(SUBB, SUBB, V1L, V1H, V2L, V2H)')

define(`SUB1n',
``;;SUB1n($1)'
CLR C
OPxn(SUBB, SUBB, V1L, V1H, LOW ($1), HIGH ($1))')

define(`SUBbpn',
``;;SUBbpn'
CLR C
OPpn(SUBB, , LOW ($1), HIGH ($1))')

define(`SUBwpn',
``;;SUBwpn($1)'
CLR C
OPpn(SUBB, SUBB, LOW ($1), HIGH ($1))')

define(`SWAP12',
``;;SWAP12'
MOV A,V1L
XCH A,V2L
MOV V1L,A
MOV A,V1H
XCH A,V2H
MOV V1H,A')

define(`SWAP1s',
``;;SWAP1s'
POP2
SWAP12
PUSH2')

define(`SWITCH',
``;;SWITCH1'
LCALL __switch')

define(`XOR12',
``;;XOR12'
OPxn(XRL, XRL, V2L, V2H, 1, 0)')

define(`CSCINIT',
``;;CSCINIT'
LCALL __cscinit')

define(`ENTERM',
``;;ENTERM'
CLR EA')

define(`EXITM',
``;;EXITM'
SETB EA')

divert

