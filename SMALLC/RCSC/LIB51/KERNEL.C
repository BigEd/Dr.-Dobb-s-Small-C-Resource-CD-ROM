/*
** Concurrent Small C Multi-tasking Kernel for the 8051
** Copyright 1997 Andy W. K. Yuen
** All rights reserved.
**
** Please note that for efficiency reasons, the kernel is
** significantly different from the 80x86 version.
** However, the overall architecture remains unchanged.
*/
#include <stdio.h>

/* common data types */
#define UINT		unsigned int
#define UCHAR		unsigned char

/* processor-dependent implementation: 8051 version */
#define MAXINTR		12
#define TIMERINTR	1
#define SERIALINTR	4

/* task descriptor structure and constants */
#define TD_NEXT         0	/* points to next descriptor */
#define TD_PRIOR        1	/* task priority */
#define TD_DELTA	2	/* delta value for Delay */
#define TD_HSP		3	/* hardware stack pointer */
#define TD_CONTEXT	4	/* start of task context */
#define TD_SP           TD_CONTEXT+4	/* task's stack pointer */
#define TD_BP		TD_CONTEXT+5	/* task base pointer */
#define TD_STKAREA	TD_CONTEXT+12	/* hardware stack area */

#define TD_SIZE         (32 * sizeof(int))	/* task descriptor size */

#define PRIOR           64	/* default task priority */
#define FLAGS           0x10	/* initial flag register value: use bank 2 */

#define BUFSIZE		128	/* read/write buffer size */

/* part of the interrupt descriptor has been moved to code segment */
static UINT intrcond[MAXINTR];	/* interrupt condition variable */
static UINT intrerror[MAXINTR];	/* interrupt error variable */

/* kernel linked lists */
static UINT *running;	/* running task */
static UINT *runcopy;	/* copy of running task descriptor */
static UINT *ready;	/* ready task list */
static UINT *delta;	/* delta list for time delay processing */

/* serial comm monitor variables */
static UCHAR *readbuf;			/* read buffer */
static UCHAR *writebuf;			/* write buffer */
static UINT readhead, readtail, readerror;
static UINT writehead, writetail, writeerror;
static CONDITION readwait;
static CONDITION writewait;

/* *******************************************************
	start of general kernel functions 
******************************************************** */

/* wait on condition: used inside a monitor */
void Wait(cond)
UINT *cond;
{
	runcopy = running;
	_Insert(cond, running);

	running = ready;
	ready = running[TD_NEXT];
	_Transfer();
}


/* signal condition: used inside a monitor */
void Signal(cond)
UINT *cond;
{
	if (*cond)
		{
		runcopy = running;
		_Insert(&ready, running);

		running = *cond;
		*cond = running[TD_NEXT];
		_Transfer();
		}
}


/* put running task in ready queue and dispatch first one in ready queue */
void Yield()
{
	disable();
	if (ready)
		_Preempt(&ready);
	enable();
}


/* delay for the specified number of ticks */
Delay(ticks)
UINT ticks;
{
	UINT *last, *current;

	if (ticks <= 0)
		return 0;

	disable();

	/* insert into delta list */
	last = 0; current = delta;
	while ((current) && (ticks >= current[TD_DELTA]))
	{
		last = current;
		current = current[TD_NEXT];
		ticks -= last[TD_DELTA];
	}

	running[TD_NEXT] = current;
	running[TD_DELTA] = ticks;

	if (last)
		last[TD_NEXT] = running;
	else
		delta = running;

	_Preempt(NULL);

	enable();

	return 0;
}


/* initiate i/o operation and make requesting task wait in interrupt 
	condition variable. Return -1 in retcode if no such interrupt 
	number in interrupt list */
void StartIO(intrnum, fn, retcode)
UINT intrnum; int (*fn)(); int *retcode;
{
	UINT *entry;
	UINT errcode;

	disable();

	/* initiate i/o operation and wait */
	if (intrnum <= MAXINTR) {
		if (fn) {
			if ((errcode = (*fn)()) == 0) {
				Wait(&intrcond[intrnum]);
				}
			else {
				intrerror[intrnum] = errcode;
				}
			}
		else
			Wait(&intrcond[intrnum]);

		*retcode = intrerror[intrnum];
		}
	else
		*retcode = -1;

	enable();
}

/* get the priority of running task */
GetPrior()
{
	return running[TD_PRIOR];
}

/* set the priority of running task */
void SetPrior(prior)
UINT prior;
{
	running[TD_PRIOR] = prior;
}

/* get a byte from input port */
inpbyte(port)
UINT port;
{
#asm
	GETw1s(4)
	MOV	A,V1L
	MOV	R0,A
	MOV	A,@R0
	MOV	V1L,A
	MOV	V1H,#0
#endasm
}

/* write a byte to output port */
void outpbyte(port, value)
UINT port; int value;
{
#asm
	GETw1s(6)
	MOV	A,V1L
	MOV	R0,A
	GETw1s(4)
	MOV	A,V1L
	MOV	@R0,A
#endasm
}

/* disable interrupt */
void disable()
{
#asm
	CLR	EA
#endasm
}


/* enable interrupt */
void enable()
{
#asm
	SETB	EA
#endasm
}

/* *******************************************************
	start of internal kernel functions 
******************************************************** */

/* insert task descriptor into specified queue according to priority.
	The larger the priority value, the higher the priority. */
void _Insert(queue, td)
UINT *queue; UINT *td;
{
	UINT *last, *current;

	last = 0; current = *queue;
	while ((current) && (current[TD_PRIOR] >= td[TD_PRIOR]))
	{
		last = current;
		current = current[TD_NEXT];
	}

	td[TD_NEXT] = current;

	if (last)
		last[TD_NEXT] = td;
	else
		*queue = td;
}

/* move the running task to toq and dispatch first task from ready */
void _Preempt(toq)
UINT *toq;
{
	runcopy = running;
	if (toq)
		_Insert(toq, running);

	running = ready;
	ready = running[TD_NEXT];
	_Transfer();
}

/* process delta list and move delayed tasks to ready queue if it is time */
interrupt _dodelta(TIMERINTR)
{
	if (delta)
		{
		if (!--delta[TD_DELTA])
			{
			UINT *current;
			UINT *last;

			current = delta;
			while ((current) && (!current[TD_DELTA]))
				{
				last = current;
				current = current[TD_NEXT];
				_Insert(&ready, last);
				}
			delta = current;
			}
		}

	/* return 1 to inform common ISR to do task switch */
	return 1;
}

extern UINT _stksize;

/* create a new task */
void _Task(fn, priority, stacksize)
UINT fn; UINT priority; UINT stacksize;
{
	UINT *block;
	UINT *ptr;
	UINT size;

	/* allocate task descriptor */
	block = calloc(1, TD_SIZE);
	block[TD_PRIOR] = (priority)? priority: PRIOR;

	/* allocate stack for task */
	size = (stacksize)? stacksize: _stksize;
	ptr = calloc(1, size);

	/* set up hardware and software stacks */
	block[TD_HSP] = 0x2a;
	block[TD_SP] = block[TD_BP] = ptr;

	/* setup fn start address and PSW */
	block[TD_STKAREA] = (fn / 256) + (fn * 256);
	block[TD_STKAREA + 1] = FLAGS * 256;

	_Insert(&ready, block);
}


/* system idle task */
void _Idle()
{
	while (1)
	{

/* It does not matter whether Idle calls Yield() or not. User and timer
	interrupts will do the job of task switching.
		Yield();
*/
	}
}


/* start multi-tasking by converting caller usually main() into a task 
	and perform a task switch */
void _Go()
{
	UINT *block;

	/* create idle task */
	_Task(_Idle, 1, NULL);

	/* make caller into the running task */
	block = calloc(1, TD_SIZE);
	block[TD_PRIOR] = PRIOR;
	running = block;

	/* initialize timer and serial communication */
	_InitHardware();

	/* start multi-tasking */
}

/* called from handler to check error code */
void _checkerr(error, intnum)
UINT error; UINT intnum;
{
	/* if user function return non-zero, it is time to wake up
		task waiting on this interrupt, if one is waiting */
	intrerror[intnum] = error;
	if (error && intrcond[intnum]) {
		if (intnum != TIMERINTR) {
			runcopy = intrcond[intnum];
			intrcond[intnum] = runcopy[TD_NEXT];
			_Insert(&ready, runcopy);
			}

		/* save task context */
		if (ready) {
			runcopy = running;
			_Insert(&ready, running);
			running = ready;
			ready = running[TD_NEXT];

			}

		}
}

/* commom interrupt handler. Every user-handled interrupt jumps here first */
#asm
__handler:
	CLR	EA		;;disable interrupts
	MOV	CDPL,DPL	;;save context of running task
	MOV	CDPH,DPH
	MOV	CP2,P2
	MOV	CA,A
	MOV	CB,B
	POP	10		;;remove user function return address
	POP	9
	PUSH	PSW		;;save PSW

	MOV	PSW,#0		;;switch to register bank 0
	GETw1m(_running)
	ADD1n(7)
	MOV	DPL,V1L
	MOV	DPH,V1H
	MOV	A,SP		;;save hardware stack pointer
	MOVX	@DPTR,A
	INC	DPTR
	MOV	R0,#10h

__handler_savecontext:
	MOV	A,@R0		;;save context
	MOVX	@DPTR,A
	INC	DPTR
	MOV	A,R0
	INC	R0
	CJNE	A,SP,__handler_savecontext

	MOV	DPH,10		;;duplicate return address
	MOV	DPL,9
	CLR	A
	MOVC	A,@A+DPTR
	MOV	V1H,A
	MOV	A,#1
	MOVC	A,@A+DPTR
	MOV	V1L,A
	CALL1			;;invoke user interrupt function
	PUSH1

	MOV	DPH,10
	MOV	DPL,9
	MOV	A,#2
	MOVC	A,@A+DPTR	;;get interrupt number
	MOV	V1H,A
	MOV	A,#3
	MOVC	A,@A+DPTR
	MOV	V1L,A
	PUSH1
	ARGCNTn(2)
	LCALL	__checker	;;check user function error code
	ADDSP(4)

	;;restore context of task to run next
	GETw1m(_running)
	ADD1n(7)
	MOV	DPL,V1L
	MOV	DPH,V1H
	MOVX	A,@DPTR
	MOV	SP,A		;;restore hardware stack pointer
	INC	DPTR
	MOV	R0,#10h
__handler_restorecontext:
	MOVX	A,@DPTR		;;restore context
	MOV	@R0,A
	INC	DPTR
	MOV	A,R0
	INC	R0
	CJNE	A,SP,__handler_restorecontext

	MOV	DPL,CDPL
	MOV	DPH,CDPH
	MOV	P2,CP2
	MOV	A,CA
	MOV	B,CB
	POP	PSW		;;restore PSW
	SETB	EA		;;enable interupts
	RETI
#endasm

/* concurrent Small C initialization: monitor initialization, 
	task declaration, interrupt handler setup, convert main 
	into a task and start multi-tasking */
void _cscinit ()
{

#asm
	;;scan monitor table for entries and call each function
	MOV	8,#mon_size / 3
	CLR	A
	ORL	A,8
	JZ	__scantask
	MOV	DPTR,#mon_start
__nextmon:
	;;save DPTR
	MOV	9,DPH
	MOV	10,DPL
	;;set up return address
	MOV	DPTR,#__monresume
	PUSH	DPL
	PUSH	DPH
	MOV	DPH,9
	MOV	DPL,10
	CLR	A
	JMP	@A+DPTR
__monresume:
	;;restore DPTR
	MOV	DPH,9
	MOV	DPL,10
	INC	DPTR
	INC	DPTR
	INC	DPTR
	DJNZ	8,__nextmon

__scantask:
	;;scan task table for entries and create task
	MOV	8,#task_size / 6
	CLR	A
	ORL	A,8
	JZ	__taskdone
	MOV	DPTR,#task_start
__nexttask:
	;;save DPTR
	MOV	9,DPH
	MOV	10,DPL
	;;set up parameters for task creation
	CLR	A
	MOVC	A,@A+DPTR
	MOV	V1H,A
	MOV	A,#1
	MOVC	A,@A+DPTR
	MOV	V1L,A
	PUSH1
	MOV	A,#2
	MOVC	A,@A+DPTR
	MOV	V1H,A
	MOV	A,#3
	MOVC	A,@A+DPTR
	MOV	V1L,A
	PUSH1
	MOV	A,#4
	MOVC	A,@A+DPTR
	MOV	V1H,A
	MOV	A,#5
	MOVC	A,@A+DPTR
	MOV	V1L,A
	PUSH1
	ARGCNTn(3)
	LCALL	__Task
	ADDSP(6)

	;;restore DPTR
	MOV	DPH,9
	MOV	DPL,10
	INC	DPTR
	INC	DPTR
	INC	DPTR
	INC	DPTR
	INC	DPTR
	INC	DPTR
	DJNZ	8,__nexttask

__taskdone:
#endasm

	/* set up timer condition variable*/
	intrcond[TIMERINTR] = ready;

	/* setup timers and serial comm for multi-tasking */
	_Go();

}

/* initialize hardware facilitites: timere and serial comm. */
void _InitHardware()
{
#asm
	;;timer0 periodic interrupt used for multitasking
	;;and timer1 used as baud rate generator for serial comm
	MOV	TMOD,#21h
	MOV	TCON,#55h
	MOV	TH1,#0f3h	;;2400 baud
	MOV	SCON,#50h	;;serial comm mode 1, 8 data bits
	MOV	IE,#92h		;;enable interrupts
#endasm
}


/* switch from the task whose descriptor is in runcopy to that in running.
	runcopy and running have been set up by caller.
	Interrupts have been disabled before it gets here */
#asm
public __Transfe:
	MOV	CDPL,DPL	;;save context of running task
	MOV	CDPH,DPH
	MOV	CP2,P2
	MOV	CA,A
	MOV	CB,B
	PUSH	PSW		;;save PSW

	MOV	PSW,#0		;;switch to register bank 0
	GETw1m(_runcopy)
	ADD1n(7)
	MOV	DPL,V1L
	MOV	DPH,V1H
	MOV	A,SP		;;save hardware stack pointer
	MOVX	@DPTR,A
	INC	DPTR
	MOV	R0,#10h
__savecontext:
	MOV	A,@R0		;;save context
	MOVX	@DPTR,A
	INC	DPTR
	MOV	A,R0
	INC	R0
	CJNE	A,SP,__savecontext

	;;restore context of task to run next
	GETw1m(_running)
	ADD1n(7)
	MOV	DPL,V1L
	MOV	DPH,V1H
	MOVX	A,@DPTR
	MOV	SP,A		;;restore hardware stack pointer
	INC	DPTR
	MOV	R0,#10h
__restorecontext:
	MOVX	A,@DPTR		;;restore context
	MOV	@R0,A
	INC	DPTR
	MOV	A,R0
	INC	R0
	CJNE	A,SP,__restorecontext

	MOV	DPL,CDPL
	MOV	DPH,CDPH
	MOV	P2,CP2
	MOV	A,CA
	MOV	B,CB
	POP	PSW		;;restore PSW
	RETI

#endasm

/* *******************************************************
	start of kernel serial comm monitor 
******************************************************** */
void monitor serial()
{
	/* allocate memory for buffers */
	readbuf = calloc(1, BUFSIZE);
	writebuf = calloc(1, BUFSIZE);
}

/* write a char, suspend execution if write buffer is full */
entry putchar(ch)
UINT ch;
{
UINT index;

	if ((index = (writetail + 1) % BUFSIZE) == writehead) {
		/* buffer full */
		Wait(&writewait);
		}

	if (writetail == writehead) {
		/* buffer empty, send character */
		return _xmit(ch);
		}
	else {
		/* store in circular buffer */
		writetail = index;
		writebuf[writetail] = ch;
		}

	return ch;
}

/* read a char, suspend execution if read buffer is empty */
entry getchar()
{
UINT ch;

	if (readtail == readhead) {
		/* buffer empty */
		Wait(&readwait);
		}

	/* store in circular buffer */
	ch = readbuf[readhead];
	readhead = (readhead + 1) % BUFSIZE;
	return ch;
}

/* interrupt service routine for serial comm */
interrupt _serialisr(SERIALINTR)
{
#asm
	JNB	RI,__chkxmit
	;;get char from serial port
	MOV	A,SBUF
	CLR	RI
	MOV	V1L,A
	CLR	A
	MOV	V1H,A
	PUSH1
	ARGCNTn(1)
	LCALL	__putread	;;put in circular buffer
	ADDSP(2)
__chkxmit:
	JNB	TI,__isrdone
	;;ready to transmit
	CLR	TI
	LCALL	__getwrit	;;get char from circular buffer
	MOV	A,V1H
	JNB	ACC.7,__isrdone	;;send only when buffer is not empty
	MOV	A,V1L
	MOV	SBUF,A
__isrdone:
#endasm
}

/* put char in read circular buffer and wake up waiting process */
_putread(ch)
UCHAR ch;
{
UINT index;

	if ((index = (readtail + 1) % BUFSIZE) != readhead) {
		/* store in circular buffer */
		readbuf[index] = ch;
		Signal(&readwait);
		return ch;
		}

	/* buffer full */
	return EOF;
}

/* get char from write circular buffer and wake up waiting process */
_getwrite()
{
UINT empty, ch;

	if (writetail != writehead) {
		/* buffer not empty */
		ch = writebuf[writehead];
		writehead = (writehead + 1) % BUFSIZE;
		Signal(&writewait);
		return ch;
		}

	/* buffer empty */
	return EOF;
}

/* transmit one char */
_xmit(ch)
UCHAR ch;
{
#asm
	GETw1s(-1)
	MOV	A,V1L
	MOV	SBUF,A
#endasm
}

/* *******************************************************
	start of kernel startup code
	Always place at end of module.
******************************************************** */

#asm
;;startup code
extern code _main


;;hardware stack starting address
ISTACK	equ 0x25


public __startup:
	MOV	IE,#0		;;disable all interrupts
	MOV	SP,#ISTACK
	MOV	PSW,#10h	;;use register bank 2
	ACALL	__initvar
	;;set up software stack according to returned value
	MOV	VSPL,A
	MOV	VSPH,B
	MOV	VBPL,VSPL
	MOV	VBPH,VSPH
	LCALL	_main
	SJMP	__startup	;;should not get here at all

;;make sure no data variable starts at location 0 because
;;the NULL pointer is defined as 0
;;
	

__initvar:
;;copy constants stored in code segment to xdata segment
;;you should not start the watchdog timer before this is done
;;if you have no control over this then you have to insert code 
;;to reset the watchdog timer while copying to avoid reset

;;set up addresses and number of bytes to copy
	MOV	R2,#LOW cvar_size
	MOV	R3,#(HIGH cvar_size) + 1
	MOV	DPTR,#CLITBEG
	MOV	R0,#LOW DVARBEG
	MOV	P2,#HIGH DVARBEG
more:
	DEC	R3
nextbyte:
	MOV	A,#0
	MOVC	A,@A+DPTR
	MOVX	@R0,A

;;increment addresses
	INC	DPTR
	INC	R0
	MOV	A,R0
	JNZ	check
	INC	P2

;;check if done yet
check:
	DEC	R2
	MOV	A,R2
	JZ	check_done
	SJMP	nextbyte
check_done:
	MOV	A,R3
	JNZ	more

	;;finished copying
	;;initialize system constants:
	;; __memptr, __memend and __stksize
	MOV	DPTR,#__memptr
	MOV	A,# HIGH (DVARBEG+cvar_size+STKSIZE+6)
	MOVX	@DPTR,A
	INC	DPTR
	MOV	A,# LOW (DVARBEG+cvar_size+STKSIZE+6)
	MOVX	@DPTR,A
	INC	DPTR
	MOV	A,# HIGH (LASTRAM)
	MOVX	@DPTR,A
	INC	DPTR
	MOV	A,# LOW (LASTRAM)
	MOVX	@DPTR,A
	INC	DPTR
	MOV	A,# HIGH (STKSIZE)
	MOVX	@DPTR,A
	INC	DPTR
	MOV	A,# LOW (STKSIZE)
	MOVX	@DPTR,A
	;;allocate stack area for main()
	MOV	B,# HIGH (DVARBEG+cvar_size+6)
	MOV	A,# LOW (DVARBEG+cvar_size+6)
	RET

;;set cold start vectors

VECTOR=32
	SEG CODE AT 0
	LJMP	__startup


SEG XDATA AT DVARBEG+cvar_size
public __memptr:	;;free memory pointer
	RW	1
public __memend:	;;end of free memory
	RW	1
public __stksize:	;;default stack size
	RW	1


#endasm
