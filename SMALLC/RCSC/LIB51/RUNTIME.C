/*
** Concurrent Small C 8051 runtime library
** Copyright 1997 Andy W. K. Yuen
** All rights reserved.
**
** This library contains all mandatory routines including
** conditional branching, multiplication, division, etc.
** for the 8051.
*/
#asm

SEG CODE

public __addsp:
	MOV	A,DPL
	ADD	A,VSPL
	MOV	VSPL,A
	MOV	A,DPH
	ADDC	A,VSPH
	MOV	VSPH,A
	RET

public __enter:
	;;save return address onto external stack
	;;low bye followed by high byte (different
	;;from 8051 stack order
	MOV	DPL,VSPL
	MOV	DPH,VSPH
	INC	DPTR
	MOVX	@DPTR,A
	INC	DPTR
	MOV	A,B
	MOVX	@DPTR,A
	;;save virtual BP pointer onto external stack
	INC	DPTR
	MOV	A,VBPH
	MOVX	@DPTR,A
	INC	DPTR
	MOV	A,VBPL
	MOVX	@DPTR,A
	;;save stack and BP pointers
	MOV	VBPL,VSPL
	MOV	VBPH,VSPH
	MOV	VSPL,DPL
	MOV	VSPH,DPH
	RET

public __return:
	;;restore SP
	MOV	A,DPL
	ADD	A,VSPL
	MOV	VSPL,A
	MOV	A,DPH
	ADDC	A,VSPH
	MOV	VSPH,A
	MOV	DPL,VBPL
	MOV	DPH,VBPH
	;;push return address onto internal stack
	INC	DPTR
	MOVX	A,@DPTR
	PUSH	ACC
	INC	DPTR
	MOVX	A,@DPTR
	PUSH	ACC
	;;restore old BP
	INC	DPTR
	MOVX	A,@DPTR
	MOV	VBPH,A
	INC	DPTR
	MOVX	A,@DPTR
	MOV	VBPL,A
	RET

public __call1:		
	MOV	A,V1L
	PUSH	ACC
	MOV	A,V1H
	PUSH	ACC
__call1ret:
	RET

public __switch:
	;;get switch table address
	POP	ACC
	MOV	DPH,A
	POP	ACC
	MOV	DPL,A
	;;check for end of table
checkend:
	CLR	A
	MOVC	A,@A+DPTR
	JNZ	chkval
	MOV	A,#1
	MOVC	A,@A+DPTR
	JZ	done
chkval:
	;;check case value
	CLR	C
	MOV	A,#3
	MOVC	A,@A+DPTR
	SUBB	A,V1L
	JNZ	next
	MOV	A,#2
	MOVC	A,@A+DPTR
	SUBB	A,V1H
	JNZ	next
	MOV	A,#1
	MOVC	A,@A+DPTR
	PUSH	ACC
	CLR	A
	MOVC	A,@A+DPTR
	PUSH	ACC
	RET
next:
	INC	DPTR
	INC	DPTR
	INC	DPTR
	INC	DPTR
	SJMP	checkend
done:
	;;jump to default or continuation
	MOV	A,#2
	JMP	@A+DPTR

;;data access operations
public __getb1p:
	MOV	A,DPL
	ADD	A,V2L
	MOV	DPL,A
	MOV	A,DPH
	ADDC	A,V2H
	MOV	DPH,A
	MOVX	A,@DPTR
	MOV	V1L,A
	;;sign extend register
	MOV	V1H,#0
	JNB	ACC.7,$+5
	MOV	V1H,#-1
	RET

public __getb1s:
	MOV	A,DPL
	ADD	A,VBPL
	MOV	DPL,A
	MOV	A,DPH
	ADDC	A,VBPH
	MOV	DPH,A
	MOVX	A,@DPTR
	MOV	V1L,A
	;;sign extend register
	MOV	V1H,#0
	JNB	ACC.7,$+5
	MOV	V1H,#-1
	RET

public __getb1su:	
	MOV	A,DPL
	ADD	A,VBPL
	MOV	DPL,A
	MOV	A,DPH
	ADDC	A,VBPH
	MOV	DPH,A
	MOVX	A,@DPTR
	MOV	V1L,A
	RET

public __getw1p:
	MOV	A,DPL
	ADD	A,V2L
	MOV	DPL,A
	MOV	A,DPH
	ADDC	A,V2H
	MOV	DPH,A
	MOVX	A,@DPTR
	MOV	V1H,A
	INC	DPTR
	MOVX	A,@DPTR
	MOV	V1L,A
	RET

public __getw1s:
	MOV	A,DPL
	ADD	A,VBPL
	MOV	DPL,A
	MOV	A,DPH
	ADDC	A,VBPH
	MOV	DPH,A
	MOVX	A,@DPTR
	MOV	V1H,A
	INC	DPTR
	MOVX	A,@DPTR
	MOV	V1L,A
	RET

public __getw2p:	
	MOV	A,DPL
	ADD	A,V2L
	MOV	DPL,A
	MOV	A,DPH
	ADDC	A,V2H
	MOV	DPH,A
	MOVX	A,@DPTR
	MOV	V2H,A
	INC	DPTR
	MOVX	A,@DPTR
	MOV	V2L,A
	RET

public __getw2s:
	MOV	A,DPL
	ADD	A,VBPL
	MOV	DPL,A
	MOV	A,DPH
	ADDC	A,VBPH
	MOV	DPH,A
	MOVX	A,@DPTR
	MOV	V2H,A
	INC	DPTR
	MOVX	A,@DPTR
	MOV	V2L,A
	RET

public __point1s:
	MOV	A,DPL
	ADD	A,VBPL
	MOV	V1L,A
	MOV	A,DPH
	ADDC	A,VBPH
	MOV	V1H,A
	RET

public __point2s:
	MOV	A,DPL
	ADD	A,VBPL
	MOV	V2L,A
	MOV	A,DPH
	ADDC	A,VBPH
	MOV	V2H,A
	RET

;;logical negate
public __lneg1:		
	MOV	A,V1L
	ORL	A,V1H
	JNZ	__setfalse
__settrue:
	MOV	V1L,#1
	MOV	V1H,#0
	RET
__setfalse:
	MOV	V1L,#0
	MOV	V1H,#0
	RET

;;comparison operators

public __eq12:	
	CLR	C
	MOV	A,V1L
	SUBB	A,V2L
	JZ	$+5
	LJMP	__setfalse
	MOV	A,V1H
	SUBB	A,V2H
	JZ	$+5
	LJMP	__setfalse
	MOV	V1L,#1
	MOV	V1H,#0
	RET

public __ne12:		
	LCALL	__eq12
	MOV	A,V1L
	XRL	A,#1
	MOV	V1L,A
	RET

public __lt12:		
	MOV	A,V1H
	XRL	A,V2H
	JB	ACC.7,$+6
	LJMP	__ult12
	MOV	A,V2H
	JNB	ACC.7,$+6
	LJMP	__settrue
	LJMP	__setfalse

public __ge12:		
	LCALL	__lt12
	MOV	A,V1L
	XRL	A,#1
	MOV	V1L,A
	RET

public __le12:		
	MOV	A,V1H
	XRL	A,V2H
	JB	ACC.7,$+6
	LJMP	__ule12
	MOV	A,V2H
	JNB	ACC.7,$+6
	LJMP	__settrue
	LJMP	__setfalse

public __gt12:		
	LCALL	__le12
	MOV	A,V1L
	XRL	A,#1
	MOV	V1L,A
	RET

public __ult12:		
	CLR	C
	MOV	A,V2L
	SUBB	A,V1L
	MOV	A,V2H
	SUBB	A,V1H
	JC	$+5
	LJMP	__setfalse
	MOV	V1L,#1
	MOV	V1H,#0
	RET

public __uge12:		
	LCALL	__ult12
	MOV	A,V1L
	XRL	A,#1
	MOV	V1L,A
	RET

public __ugt12:		
	LCALL	__ule12
	MOV	A,V1L
	XRL	A,#1
	MOV	V1L,A
	RET

public __ule12:		
	CLR	C
	MOV	A,V2L
	SUBB	A,V1L
	MOV	A,V2H
	SUBB	A,V1H
	JC	$+5
	LJMP	__eq12
	MOV	V1L,#1
	MOV	V1H,#0
	RET

;;arithmetic operations
public __asl12:		
	MOV	A,V1L
	MOV	VLCNT,A
	MOV	A,V2L
	MOV	V1L,A
	MOV	A,V2H
	MOV	V1H,A
aslloop:
	CLR	C
	MOV	A,V1L
	RLC	A
	MOV	V1L,A
	MOV	A,V1H
	RLC	A
	MOV	V1H,A
	DJNZ	VLCNT,aslloop
	RET

public __asr12:		
	MOV	A,V1L
	MOV	VLCNT,A
	MOV	A,V2L
	MOV	V1L,A
	MOV	A,V2H
	MOV	V1H,A
asrloop:
	CLR	C
	MOV	A,V1L
	RRC	A
	MOV	V1L,A
	MOV	A,V1H
	RRC	A
	MOV	V1H,A
	DJNZ	VLCNT,asrloop
	RET

public __udiv12:	
	LCALL	__div16
	MOV	V1L,VR2
	MOV	V1H,VR3
	RET

public __umul12:	
	LCALL	__mul16
	MOV	V1L,VR0
	MOV	V1H,VR1
	RET

public __umod12:	
	LCALL	__div16
	MOV	V1L,VR0
	MOV	V1H,VR1
	RET

public __mul12:		
	;;save V2
	MOV	A,V2H
	PUSH	ACC
	MOV	A,V2L
	PUSH	ACC
	;;convert to sign/magnitude format and multiply
	LCALL	__cv122sm
	LCALL	__umul12
	;;convert back to 2's complement format
	JB	V1SIGN,mul_tst2
	JB	V2SIGN,mul_docv
	;;both V1 and V2 positive
mul_docpy:
	MOV	A,VR0
	MOV	B,VR1
	SJMP	mul_save1
mul_tst2:
	JB	V2SIGN,mul_docpy
mul_docv:
	;;convert back to 2's complement format
	SETB	VRSIGN
	LCALL	__cvsm2tc
mul_save1:
	MOV	V1L,A
	MOV	V1H,B
	POP	ACC
	MOV	V2L,A
	POP	ACC
	MOV	V2H,A
	RET

public __div12:		
	;;save V2
	MOV	A,V2H
	PUSH	ACC
	MOV	A,V2L
	PUSH	ACC
	;;convert to sign/magnitude format and multiply
	LCALL	__cv122sm
	LCALL	__udiv12
	;;move quotient to expected place for conversion
	MOV	A,VR2
	MOV	VR0,A
	MOV	A,VR3
	MOV	VR1,A
	;;convert back to 2's complement format
	JB	V1SIGN,div_tst2
	JB	V2SIGN,div_docv
	;;both V1 and V2 positive
div_docpy:
	MOV	A,VR0
	MOV	B,VR1
	SJMP	div_save1
div_tst2:
	JB	V2SIGN,div_docpy
div_docv:
	;;convert back to 2's complement format
	SETB	VRSIGN
	LCALL	__cvsm2tc
div_save1:
	MOV	V1L,A
	MOV	V1H,B
	POP	ACC
	MOV	V2L,A
	POP	ACC
	MOV	V2H,A
	RET

public __mod12:		
	;;save V2
	MOV	A,V2H
	PUSH	ACC
	MOV	A,V2L
	PUSH	ACC
	;;convert to sign/magnitude format and multiply
	LCALL	__cv122sm
	LCALL	__udiv12
	;;convert back to 2's complement format
	JB	V1SIGN,mod_tst2
	JB	V2SIGN,mod_docv
	;;both V1 and V2 positive
mod_docpy:
	MOV	A,VR0
	MOV	B,VR1
	SJMP	mod_save1
mod_tst2:
	JB	V2SIGN,mod_docpy
mod_docv:
	;;convert back to 2's complement format
	SETB	VRSIGN
	LCALL	__cvsm2tc
mod_save1:
	MOV	V1L,A
	MOV	V1H,B
	POP	ACC
	MOV	V2L,A
	POP	ACC
	MOV	V2H,A
	RET

;;stack operations
public __pushm:		
	MOVX	A,@DPTR
	MOV	B,A
	INC	DPTR
	MOVX	A,@DPTR
	XCH	A,B
	LJMP	__pushab

public __pushp:		
	MOV	DPL,V2L
	MOV	DPH,V2H
	LJMP	__pushm

public __pushs:		
	MOV	A,DPL
	ADD	A,VBPL
	MOV	DPL,A
	MOV	A,DPH
	ADDC	A,VBPH
	MOV	DPH,A
	LJMP	__pushm

public __push1:
	MOV	A,V1H
	MOV	B,V1L
	LJMP	__pushab

public __push2:
	MOV	A,V2H
	MOV	B,V2L
	LJMP	__pushab

public __pop2:
	CLR	C
	MOV	A,VSPL
	SUBB	A,#2
	MOV	VSPL,A
	MOV	A,VSPH
	SUBB	A,#0
	MOV	VSPH,A
	MOV	DPL,VSPL
	MOV	DPH,VSPH
	INC	DPTR
	MOVX	A,@DPTR
	MOV	V2H,A
	INC	DPTR
	MOVX	A,@DPTR
	MOV	V2L,A
	RET

;;misc functions
public _ccargc:
	MOV	A,VACNT
	MOV	V1L,A
	MOV	V1H,#0
	RET

;;local support routines
__pushab:
	MOV	DPL,VSPL
	MOV	DPH,VSPH
	INC	DPTR
	MOVX	@DPTR,A
	INC	DPTR
	MOV	A,B
	MOVX	@DPTR,A
	MOV	VSPL,DPL
	MOV	VSPH,DPH
	RET

__mul16:	;;VR <- V1 * V2
	;;note: V1 contents destroyed during calculation
	;;muliticand in V1, multiplier in V2
	;;results saved in VR

	;;multiply 16-bit V1 by 8-bit V2L
	MOV	A,V2L
	MOV	B,V1L
	MUL	AB
	MOV	VR0,A
	MOV	R0,B
	MOV	A,V2L
	MOV	B,V1H
	MUL	AB
	ADD	A,R0
	MOV	VR1,A
	CLR	A
	ADDC	A,B
	MOV	VR2,A

	;;multiply 16-bit V1 by 8-bit V2H
	MOV	A,V2H
	MOV	B,V1L
	MUL	AB
	MOV	V1L,A
	MOV	R0,B
	MOV	A,V2H
	MOV	B,V1H
	MUL	AB
	ADD	A,R0
	MOV	V1H,A
	CLR	A
	ADDC	A,B
	MOV	R0,A

	;;add the partial sums
	MOV	A,VR1
	ADD	A,V1L
	MOV	VR1,A
	MOV	A,VR2
	ADDC	A,V1H
	MOV	VR2,A
	CLR	A
	ADDC	A,R0
	MOV	VR3,A
	RET

__div16:	;;VR <- V1 / V2
	;;note: V1 contents destroyed during calculation
	;;dividend in V1, divisor in V2
	;;quotient saved in VR2/3, remainder saved in VR0/1
	MOV	VR0,#0
	MOV	VR1,#0
	MOV	VR2,#0
	MOV	VR3,#0
	MOV	VLCNT,#16
divloop:
	;;shift dividend
	CLR	C
	MOV	A,V1L
	RLC	A
	MOV	V1L,A
	MOV	A,V1H
	RLC	A
	MOV	V1H,A
	;;shift partial remainder
	MOV	A,VR0
	RLC	A
	MOV	VR0,A
	MOV	A,VR1
	RLC	A
	MOV	VR1,A
	;;perform subtration to see if partial remainder > divisor
	CLR	C
	MOV	A,VR0
	SUBB	A,V2L
	MOV	R0,A
	MOV	A,VR1
	SUBB	A,V2H
	MOV	R1,A
	CPL	C
	JNC	addbit
	;;update partial remainder
	MOV	VR0,R0
	MOV	VR1,R1
addbit:
	;;update partial quotient
	MOV	A,VR2
	RLC	A
	MOV	VR2,A
	MOV	A,VR3
	RLC	A
	MOV	VR3,A
	DJNZ	VLCNT,divloop
	RET

__cvtc2sm:	;;convert 2's complement to sign/magnitude format
	;;A, B contains low and high byte of 2's complement
	;;VR0,VR1 contains magnitude and VRSIGN sign on return
	JB	ACC.7,cv2sm
	CLR	VRSIGN
	MOV	VR0,A
	MOV	VR1,B
	RET
cv2sm:
	SETB	VRSIGN
	CPL	A
	ADD	A,#1
	MOV	VR0,A
	MOV	A,B
	CPL	A
	ADDC	A,#0
	MOV	VR1,A
	RET

__cvsm2tc:	;;convert sign/magnitude to 2's complement format
	;;VR0,VR1 contains magnitude and VRSIGN sign
	;;A, B contains low and high byte of 2's complement on return
	JB	VRSIGN,cv2tc
	MOV	A,VR0
	MOV	B,VR1
	RET
cv2tc:
	MOV	A,VR0
	CPL	A
	ADD	A,#1
	MOV	B,A
	MOV	A,VR1
	CPL	A
	ADDC	A,#0
	XCH	A,B
	RET

__cv122sm:
	;;convert V1 to sign/magnitude format
	MOV	A,V1L
	MOV	B,V1H
	LCALL	__cvtc2sm
	CLR	V1SIGN
	JNB	VRSIGN,dov2
	SETB	V1SIGN
	MOV	V1L,VR0
	MOV	V1H,VR1
dov2:
	;;convert V2 to sign/magnitude format
	MOV	A,V2L
	MOV	B,V2H
	LCALL	__cvtc2sm
	CLR	V2SIGN
	JNB	VRSIGN,return
	SETB	V2SIGN
	MOV	V2L,VR0
	MOV	V2H,VR1
return:
	RET


#endasm

