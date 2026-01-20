/*
** All character classification functions except isascii().
** Integer argument (c) must be in ASCII range (0-127) for
** dependable answers.
**
** Modified by Andy W. K. Yuen to make it more efficient 
** for the 8051
*/

#asm

ALNUM equ	1
ALPHA equ	2
CNTRL equ	4
DIGIT equ	8
GRAPH equ	16
LOWER equ	32
PRINT equ	64
PUNCT equ	128
BLANK equ	256
UPPER equ	512
XDIGIT equ	1024

__is:
	MOV	A,V1L
	RL	A
	MOV	V1L,A
	LCALL	__getvalue
	MOV	V1H,A
	MOV	A,V1L
	INC	A
	LCALL	__getvalue
	MOV	V1L,A
	RET

__getvalue:
	CJNE	A,#255,__lookup
	MOV	A,#4
	RET
__lookup:
	INC	A
	MOVC	A,@A+PC
	RET
	
DW  0x004, 0x004, 0x004, 0x004, 0x004, 0x004, 0x004, 0x004
DW  0x004, 0x104, 0x104, 0x104, 0x104, 0x104, 0x004, 0x004
DW  0x004, 0x004, 0x004, 0x004, 0x004, 0x004, 0x004, 0x004
DW  0x004, 0x004, 0x004, 0x004, 0x004, 0x004, 0x004, 0x004
DW  0x140, 0x0D0, 0x0D0, 0x0D0, 0x0D0, 0x0D0, 0x0D0, 0x0D0
DW  0x0D0, 0x0D0, 0x0D0, 0x0D0, 0x0D0, 0x0D0, 0x0D0, 0x0D0
DW  0x459, 0x459, 0x459, 0x459, 0x459, 0x459, 0x459, 0x459
DW  0x459, 0x459, 0x0D0, 0x0D0, 0x0D0, 0x0D0, 0x0D0, 0x0D0
DW  0x0D0, 0x653, 0x653, 0x653, 0x653, 0x653, 0x653, 0x253
DW  0x253, 0x253, 0x253, 0x253, 0x253, 0x253, 0x253, 0x253
DW  0x253, 0x253, 0x253, 0x253, 0x253, 0x253, 0x253, 0x253
DW  0x253, 0x253, 0x253, 0x0D0, 0x0D0, 0x0D0, 0x0D0, 0x0D0
DW  0x0D0, 0x473, 0x473, 0x473, 0x473, 0x473, 0x473, 0x073
DW  0x073, 0x073, 0x073, 0x073, 0x073, 0x073, 0x073, 0x073
DW  0x073, 0x073, 0x073, 0x073, 0x073, 0x073, 0x073, 0x073
DW  0x073, 0x073, 0x073, 0x0D0, 0x0D0, 0x0D0, 0x0D0

DB  0x00

define(`ANDVAL',
`GETw1s(4)
LCALL	__is
OPxn(ANL, ANL, V1L, V1H, LOW ($1), HIGH ($1))')

#endasm

isalnum (c) int c; {
#asm
ANDVAL(ALNUM)
#endasm
} /* 'a'-'z', 'A'-'Z', '0'-'9' */

isalpha (c) int c; {
#asm
ANDVAL(ALPHA)
#endasm
} /* 'a'-'z', 'A'-'Z' */

iscntrl (c) int c; {
#asm
ANDVAL(CNTRL)
#endasm
} /* 0-31, 127 */

isdigit (c) int c; {
#asm
ANDVAL(DIGIT)
#endasm
} /* '0'-'9' */

isgraph (c) int c; {
#asm
ANDVAL(GRAPH)
#endasm
} /* '!'-'~' */

islower (c) int c; {
#asm
ANDVAL(LOWER)
#endasm
} /* 'a'-'z' */

isprint (c) int c; {
#asm
ANDVAL(PRINT)
#endasm
} /* ' '-'~' */

ispunct (c) int c; {
#asm
ANDVAL(PUNCT)
#endasm
} /* !alnum && !cntrl && !space */

isspace (c) int c; {
#asm
ANDVAL(BLANK)
#endasm
} /* HT, LF, VT, FF, CR, ' ' */

isupper (c) int c; {
#asm
ANDVAL(UPPER)
#endasm
} /* 'A'-'Z' */

isxdigit(c) int c; {
#asm
ANDVAL(XDIGIT)
#endasm
} /* '0'-'9', 'a'-'f', 'A'-'F' */

