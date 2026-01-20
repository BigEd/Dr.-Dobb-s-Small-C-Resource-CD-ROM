AUTO
MODE3
PRINT"****************************"
PRINT"* BBC ASCII/BOO UNARCHIVER *"
PRINT"*       (C) AUG 1989       *"
PRINT"*    S.M.Goldthorpe aka    *"
PRINT"*      The BeebBasher      *"
PRINT"****************************"
PRINT
:
REM **** INITIALISATION ****
:
ONERRORPROCerr("")
flen%=12:mcol%=76:ahdr%=1+flen%+3*9:bhdr%=1+flen%+5*9
DIMmc%768,line%255,str%255
DIMsrc%flen%,obj%flen%,fcb%17
PROCassem
:
REM **** MAIN PROGRAM ****
:
INPUT"archive file: "I$:$src%=LEFT$(I$+STRING$(flen%," "),flen%)
PROCopen_arc
REPEAT
PROCread_arc_info
PROCopen_obj
IFbinary_flag% CALLboo% ELSECALLasc_dump%
PROCtest_length
CLOSE#?out%
PROCtest_checksum
IFbinary_flag% PROCwrite_bin_info
PRINT"Done"
UNTILEOF#?inp%
CLOSE#?inp%
END
:
REM **** GENERAL ROUTINES ****
:
DEFPROCerr($str%)
IF$str%="" REPORT:CLOSE#0 ELSEPRINT
PRINT$str%
END
:
DEFFNhex(no%)
$str%=STR$~no%
REPEAT
IFLEN$str%<8 $str%="0"+$str%
UNTILLEN$str%>7
=$str%
:
DEFFNosfile(A%)
X%=fcb% AND&FF:Y%=fcb% DIV256
=USR&FFDD AND&FF
:
DEFFNyes
*FX21,1
REPEAT
key%=INSTR("YyNn",GET$)
UNTILkey%>0
IFkey%<3 PRINT"Yes" ELSEPRINT"No"
=key%<3
:
REM **** MAIN ROUTINES ****
:
DEFPROCopen_arc
?inp%=OPENIN($src%)
IF?inp%<>0 ENDPROC
PROCerr($src%+" not found")
:
DEFPROCread_arc_info
REPEAT
CALLread_line%
ok%=FALSE
IF?line%=ASC"A" ANDLEN$line%=ahdr% ok%=TRUE:PROCread_asc_info
IF?line%=ASC"B" ANDLEN$line%=bhdr% ok%=TRUE:PROCread_bin_info
IFNOTok% CLOSE#?inp%:PROCerr("corrupt - bad header")
PRINT" extract (Y/N) ";
IFFNyes ELSEFORN%=1 TO!lcnt%:CALLread_line%:NEXT:UNTILEOF#?inp%:END
UNTILTRUE
ENDPROC
:
DEFPROCread_bin_info
line%?(1+flen%)=13:$obj%=$(line%+1)
line%?(1+flen%+9)=13:load%=EVAL("&"+$(line%+2+flen%))
line%?(1+flen%+9*2)=13:exec%=EVAL("&"+$(line%+2+flen%+9))
line%?(1+flen%+9*3)=13:len%=EVAL("&"+$(line%+2+flen%+9*2))
line%?(1+flen%+9*4)=13:!lcnt%=EVAL("&"+$(line%+2+flen%+9*3))
!csum%=EVAL("&"+$(line%+2+flen%+9*4))
!wlen%=len%
PRINT'$obj%" "FNhex(load%)" "FNhex(exec%)" "FNhex(len%);
binary_flag%=TRUE
ENDPROC
:
DEFPROCread_asc_info
line%?(1+flen%)=13:$obj%=$(line%+1)
line%?(1+flen%+9)=13:len%=EVAL("&"+$(line%+2+flen%))
line%?(1+flen%+9*2)=13:!lcnt%=EVAL("&"+$(line%+2+flen%+9))
!csum%=EVAL("&"+$(line%+2+flen%+9*2))
!wlen%=len%
PRINT'$obj%" "FNhex(len%);
binary_flag%=FALSE
ENDPROC
:
DEFPROCopen_obj
?out%=OPENIN($obj%)
IF?out%<>0 PROCrename_obj
?out%=OPENOUT($obj%)
IF?out%<>0 ENDPROC
CLOSE#?inp%
PROCerr("Can't create "+$obj%)
:
DEFPROCrename_obj
REPEAT
CLOSE#?out%
PRINT"file exists"
PRINT"use a new name (y/n)? ";
IFFNyes ELSEPROCdel_obj:UNTILTRUE:ENDPROC
INPUT'"new object name: "I$:$obj%=LEFT$(I$+STRING$(flen%," "),flen%)
?out%=OPENUP($obj%)
UNTIL?out%=0
ENDPROC
:
DEFPROCdel_obj
PRINT"overwrite old file (y/n)? ";
IFFNyes ELSEEND
!fcb%=obj%
res%=FNosfile(6)
ENDPROC
:
DEFPROCtest_length
IF!wlen%=0 ANDEXT#?out%=len% PRINT'"Length O.K.":ENDPROC
CLOSE#?out%:CLOSE#?inp%
PROCerr("corrupt - length incorrect")
:
DEFPROCtest_checksum
IF!csum%=0 PRINT"Checksum O.K.":ENDPROC
CLOSE#?inp%
PROCerr("corrupt - checksum incorrect")
:
DEFPROCwrite_bin_info
!fcb%=obj%:fcb%!2=load%:fcb%!6=exec%:fcb%!14=0
res%=FNosfile(1)
ENDPROC
:
DEFPROCassem
osbput%=&FFD4:osbget%=&FFD7:oswrch%=&FFEE
fix%=ASC"0":rpt%=ASC"~"
dot%=ASC".":nl%=13
FORN%=0 TO2 STEP2
P%=mc%
[OPTN%
.boo%
LDAlcnt%:ORAlcnt%+1:ORAlcnt%+2:ORAlcnt%+3
BNEmain_loop%
RTS
.main_loop%
JSRread_line%:STXcols%
LDA#dot%:JSRoswrch%
LDX#0
.inner_loop%
LDAline%,X
CPXcols%:BCScheck_eoln%
INX
CMP#rpt%:BEQuncompress%
JMPunpack%
.check_eoln%
BNEbad_data%
CMP#nl%:BNEbad_data%
SEC:LDAlcnt%:SBC#1:STAlcnt%:BCSboo%
LDAlcnt%+1:SBC#0:STAlcnt%+1:BCSboo%
LDAlcnt%+2:SBC#0:STAlcnt%+2:BCSboo%
LDAlcnt%+3:SBC#0:STAlcnt%+3:BCSboo%
.bad_data%
]
$P%=CHR$0+CHR$99+"corrupt - bad data"+CHR$0
P%=P%+21
[OPTN%
.uncompress%
LDAline%,X:INX
SEC:SBC#fix%:STAecnt%
TXA:PHA:LDXecnt%
LDYout%
.uncomp_loop%
LDA#0:JSRosbput%
DEX:BNEuncomp_loop%
SEC:LDAwlen%:SBCecnt%:STAwlen%:BCSuncomp_end%
LDAwlen%+1:SBC#0:STAwlen%+1:BCSuncomp_end%
LDAwlen%+2:SBC#0:STAwlen%+2:BCSuncomp_end%
LDAwlen%+3:SBC#0:STAwlen%+3
.uncomp_end%
PLA:TAX
JMPinner_loop%
:
.unpack%
LDYout%
SEC:SBC#fix%:STAasc%:STAbin%
LDAline%,X:INX:SEC:SBC#fix%:STAasc%+1
LDAline%,X:INX:SEC:SBC#fix%:STAasc%+2
LDAline%,X:INX:SEC:SBC#fix%:STAasc%+3
LDAwlen%+1:ORAwlen%+2:ORAwlen%+3:STAecnt%
LSRasc%+3:RORasc%+2:RORasc%+1:RORasc%
LSRasc%+3:RORasc%+2:RORasc%+1:RORasc%
LDAasc%:AND#&C0:ORAbin%:JSRosbput%
LDAecnt%:BNEunpack_skip1%
LDAwlen%:CMP#1:BEQunpack_part%
.unpack_skip1%
LDAasc%+1:AND#&F:STAbin%+1
LSRasc%+3:RORasc%+2:RORasc%+1
LSRasc%+3:RORasc%+2:RORasc%+1
LDAasc%+1:AND#&F0:ORAbin%+1:JSRosbput%
LDAecnt%:BNEunpack_skip2%
LDAwlen%:CMP#2:BEQunpack_part%
.unpack_skip2%
LDAasc%+2:AND#3:STAbin%+2
LSRasc%+3:RORasc%+2
LSRasc%+3:RORasc%+2
LDAasc%+2:ORAbin%+2:JSRosbput%
SEC:LDAwlen%:SBC#3:STAwlen%:BCSunpack_end%
LDAwlen%+1:SBC#0:STAwlen%+1:BCSunpack_end%
:
LDAwlen%+2:SBC#0:STAwlen%+2:BCSunpack_end%
LDAwlen%+3:SBC#0:STAwlen%+3
.unpack_end%
JMPinner_loop%
.unpack_part%
LDA#0:STAwlen%
JMPinner_loop%
:
.read_line%
LDA#nl%:STAline%
LDX#0
LDYinp%
.rdln_loop%
JSRosbget%:BCSrdln_eof%
STAline%,X
CMP#nl%:BEQrdln_end%
SEC:LDAcsum%:SBCline%,X:STAcsum%:BCSrdln_skip%
LDAcsum%+1:SBC#0:STAcsum%+1:BCSrdln_skip%
LDAcsum%+2:SBC#0:STAcsum%+2:BCSrdln_skip%
LDAcsum%+3:SBC#0:STAcsum%+3
.rdln_skip%
INX
BNErdln_loop%
]
$P%=CHR$0+CHR$99+"line too long"+CHR$0
P%=P%+16
rdln_eof%=P%
$P%=CHR$0+CHR$99+"corrupt - EOF not expected"+CHR$0
P%=P%+29
[OPTN%
.rdln_end%
RTS
:
.asc_dump%
LDAlcnt%:ORAlcnt%+1:ORAlcnt%+2:ORAlcnt%+3
BNEasc_dump_loop%
RTS
.asc_dump_loop%
JSRread_line%
INX:STXcols%
LDYout%:LDX#0
.asc_dump_copy%
LDAline%,X:INX
JSRosbput%
CPXcols%:BNEasc_dump_copy%
.asc_dump_empty%
SEC:LDAwlen%:SBCcols%:STAwlen%:BCSasc_dump_skip%
LDAwlen%+1:SBC#0:STAwlen%+1:BCSasc_dump_skip%
LDAwlen%+2:SBC#0:STAwlen%+2:BCSasc_dump_skip%
LDAwlen%+3:SBC#0:STAwlen%+3
.asc_dump_skip%
LDA#dot%:JSRoswrch%
SEC:LDAlcnt%:SBC#1:STAlcnt%:BCSasc_dump%
LDAlcnt%+1:SBC#0:STAlcnt%+1:BCSasc_dump%
LDAlcnt%+2:SBC#0:STAlcnt%+2:BCSasc_dump%
LDAlcnt%+3:SBC#0:STAlcnt%+3:JMPasc_dump%
:
.asc%
BRK:BRK:BRK:BRK
.bin%
BRK:BRK:BRK:BRK
.cols%
BRK
.ecnt%
BRK
.inp%
BRK
.out%
BRK
.lcnt%
BRK:BRK:BRK:BRK
.csum%
BRK:BRK:BRK:BRK
.wlen%
BRK:BRK:BRK:BRK
]
NEXT
ENDPROC
:
REM **** END ****

