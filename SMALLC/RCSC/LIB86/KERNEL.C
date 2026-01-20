/*
** Concurrent Small C Multi-tasking Kernel
** Copyright 1996 Andy Yuen
** All rights reserved.
*/
#include <stdio.h>

/* processor-dependent implementation: 8086 version */

#define TIMERINT	0x8

/* task descriptor structure and constants */
#define TD_NEXT         0	/* points to next descriptor */
#define TD_PRIOR        1	/* task priority */
#define TD_SP           2	/* task's stack pointer */
#define TD_DELTA	3	/* delta value for Delay */

#define TD_SIZE         (4 * sizeof(int))	/* task descriptor size */

#define REGISTERS	9	/* number of registers to save */
#define STKSIZE         512	/* default task stack size */
#define PRIOR           64	/* default task priority */
#define FLAGS           0x200	/* initial flag register value */

/* interrupt descriptor structure and constants */
#define INT_NEXT	0	/* points to next descriptor */
#define INT_NUM		1	/* interrupt number */
#define INT_OPCODE	2	/* inter-segment call to common ISR */
#define INT_OFF		3
#define INT_SEG		4
#define INT_USER	5	/* address of user-defined ISR */
#define INT_COND	6	/* interrupt condition variable */
#define INT_ERROR	7	/* error variable */

/* offset of important info relative to INT_USER */
#define OF_NUM		-4
#define OF_USER		0
#define OF_COND		1
#define OF_ERROR	2

/* defines a NOP(0x90) followed by a inter-segment CALL(0x9A) instruction */
#define OPCODE		0x9A90

#define INT_SIZE	(8 * sizeof(int))	/* interrupt descriptor size */

static int *running;	/* running task */
static int *runcopy;	/* copy of running task descriptor */
static int *ready;	/* ready task list */
static int *delta;	/* delta list for time delay processing */
static int *intr;	/* interrupt descriptor list */
static int *entry;	/* temporary interrupt entry pointer */

static int timerset;	/* timer ISR set by user flag */
static int mtask;	/* multiple tasks declared flag */
static int reg_cs;	/* 8086's CS register value */
static int reg_ds;	/* 8086's DS register value */
static int critical[2]; /* DOS critical pointer */

/* insert task descriptor into specified queue according to priority.
	The larger the priority value, the higher the priority. */
void _Insert(queue, td)
int *queue; int *td;
{
	int *last, *current;

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

/* switch from the task whose descriptor is in runcopy to that in running.
	runcopy and running have been set up by caller */
void _Transfer()
{
#asm
	ADD	SP, 2			;remove stack frame
	PUSHF				;simulate an interrupt
	PUSH	_REG_CS
	MOV	AX, OFFSET _HERE	;AX destroyed, no problem
	PUSH	AX

	PUSH	AX
	PUSH	BX
	PUSH	CX
	PUSH	DX
	PUSH	SI
	PUSH	DI
	PUSH	BP
	PUSH	DS
	PUSH	ES

	MOV     BX, _RUNCOPY
	MOV     4[BX], SP

	MOV     BX,_RUNNING
	MOV     SP, 4[BX]

	POP	ES
	POP	DS
	POP	BP
	POP	DI
	POP	SI
	POP	DX
	POP	CX
	POP	BX
	POP	AX
	IRET
_HERE:
	RET
#endasm

}

/* move the running task to toq and dispatch first task from ready */
void _Preempt(toq)
int *toq;
{
	runcopy = running;
	if (toq)
		_Insert(toq, running);

	running = ready;
	ready = running[TD_NEXT];
	_Transfer();
}

/* wait on condition: used inside a monitor */
void Wait(cond)
int *cond;
{
	runcopy = running;
	_Insert(cond, running);

	running = ready;
	ready = running[TD_NEXT];
	_Transfer();
}


/* signal condition: used inside a monitor */
void Signal(cond)
int *cond;
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
int ticks;
{
	int *last, *current;

	/* return 1 if timer ISR not installed */
	if (!timerset)
		return 1;

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


/* process delta list and move delayed tasks to ready queue if it is time */
_dodelta()
{
	if (delta)
		{
		if (!--delta[TD_DELTA])
			{
			int *current;
			int *last;

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


/* create a new task */
void _Task(fn, priority, stacksize)
int (*fn)(); int priority; int stacksize;
{
	int *block;
	int *ptr;
	int size;

	/* allocate task descriptor */
	block = calloc(1, TD_SIZE);
	block[TD_PRIOR] = (priority)? priority: PRIOR;

	/* allocate stack for task */
	size = (stacksize)? stacksize: STKSIZE;
	ptr = calloc(1, size);
	ptr = block[TD_SP] = ptr + size / sizeof(int) - 
		REGISTERS - 3;

	/* set up task starting address and registers in stack */
#asm
	MOV	_REG_DS,  DS
#endasm
	*ptr = ptr[1] = reg_ds;
	ptr += REGISTERS;
#asm
	MOV	_REG_CS,  CS
#endasm
	*ptr++ = fn; *ptr++ = reg_cs; *ptr = FLAGS;
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
	int *block;

	/* create idle task */
	_Task(_Idle, 1, NULL);

	/* make caller into a task */
	block = calloc(1, TD_SIZE);
	block[TD_PRIOR] = PRIOR;

	/* start multi-tasking */
	running = block;
	if (!timerset && mtask)
		_AddIntr(_dodelta, TIMERINT);
	Yield();
}

/* commom interrupt handler. Every user-handled interrupt jumps here first */
void _handler()
{
#asm
	XCHG	AX, 4[BP]	;get segment(DS) and save AX
	XCHG	BX, 2[BP]	;get offset(USER handler) and save BX
	POP	BP		;adjust stack
	PUSH	CX		;save registers
	PUSH	DX
	PUSH	SI
	PUSH	DI
	PUSH	BP
	PUSH	DS
	PUSH	ES

	MOV	DS, AX

	PUSH	BX		;save a copy for later use
	CALL	[BX]		;invoke user interrupt function

	POP	BX
	MOV	4[BX], AX	;save error code
	MOV	_ENTRY, BX

#endasm
	/* if user function return non-zero, it is time to wake up
		task waiting on this interrupt, if one is waiting */
	if (entry[OF_ERROR] && entry[OF_COND]) {
		if (entry[OF_NUM] != TIMERINT) {
			runcopy = entry[OF_COND];
			entry[OF_COND] = runcopy[TD_NEXT];
			_Insert(&ready, runcopy);
			}
#asm
		;don't switch if DOS is in critical section
		LES	DI, DWORD PTR _CRITICAL
		TEST	BYTE PTR ES:[DI], 0FFH
		JNZ	_FIN
#endasm
		/* save task context */
		if (ready) {
			runcopy = running;
			_Insert(&ready, running);
			running = ready;
			ready = running[TD_NEXT];

#asm
			MOV     BX, _RUNCOPY
			MOV     4[BX], SP

			MOV     BX,_RUNNING
			MOV     SP, 4[BX]
#endasm
			}

		}

	/* perform time-of-day processing if this is a timer interrupt */
	if (entry[OF_NUM] == TIMERINT) {
#asm
		INT	78H
#endasm
		}
#asm
_FIN:
		MOV	AL, 20H
		OUT	20H, AL

		POP	ES
		POP	DS
		POP	BP		;restore registers
		POP	DI
		POP	SI
		POP	DX
		POP	CX
		POP	BX
		POP	AX
		IRET
#endasm

}


/* create interrupt descriptor and set up user-defined interrupt handler */
void _AddIntr(fn, num)
int (*fn)(); int num;
{
	int *block;

	/* create interrupt descriptor */
	block = calloc(1, INT_SIZE);
	block[INT_NUM] = num;
	block[INT_OPCODE] = OPCODE;
	block[INT_OFF] = _handler;
	block[INT_SEG] = reg_cs;
	block[INT_USER] = fn;
	_Insert(&intr, block);

	/* special timer ISR handling */
	if (num == TIMERINT)
		{
		block[INT_COND] = ready;
		timerset = 1;
		/* need to save old interrupt vector for invocation 
			to do time-of-day processing. interrupt
			0x78 is used for storing it */
#asm
		PUSH	DS
		PUSH	DS
		MOV	AX, 3508H
		INT	21H
		MOV	AX, 2578H
		PUSH	ES
		POP	DS
		MOV	DX, BX
		INT	21H
		POP	DS
		POP	ES
#endasm
		}

	/* install ISR */
#asm
	MOV	DX, -2[BP]	;address is &block[INT_OPCODE]
	ADD	DX, 4
	MOV	AX, 4[BP]	;interrupt number
	MOV	AH, 25H
	INT	21H
#endasm
}

/* concurrent Small C initialization: monitor initialization, 
	task declaration, interrupt handler setup, convert main 
	into a task and start multi-tasking */
void _cscinit ()
{
	int reges;

#asm
	MOV	AX, DS
	MOV	-2[BP], ES
	PUSH	ES

MNTRS SEGMENT WORD PUBLIC
MNTRS ENDS
	;scan monitor table for entries and call function if non-zero
	MOV	AX, MNTRS
	MOV	ES, AX
	MOV	SI, -2
_NXTMON:
	ADD	SI, 2
	MOV	BX, ES:[SI]
	CMP	BX, 0
	JZ	_NXTMON
	CMP	BX, 1A1AH
	JZ	_TASKTAB
	PUSH	SI
	PUSH	ES
	MOV	ES, -2[BP]

	CALL	BX

	POP	ES
	POP	SI
	JMP	_NXTMON

_TASKTAB:

TASKS SEGMENT WORD PUBLIC
TASKS ENDS
	;scan task table for entries and create task
	MOV	AX, TASKS
	MOV	ES, AX
	MOV	SI, -6
_NXTTASK:
	ADD	SI, 6
_SKIPADD:
	MOV	BX, ES:[SI]
	CMP	BX, 0
	JNZ	_NXTCMP
	ADD	SI, 2
	JMP	_SKIPADD
_NXTCMP:
	CMP	BX, 1A1AH
	JZ	_INTRTAB
	PUSH	SI
	PUSH	ES

	PUSH	BX
	MOV	AX, ES:2[SI]
	PUSH	AX
	MOV	AX, ES:4[SI]
	PUSH	AX
	MOV	CL, 3
	CALL	__TASK
	ADD	SP, 6
	INC	_MTASK		;set multi-task flag

	POP	ES
	POP	SI
	JMP	_NXTTASK
_INTRTAB:

INTRS SEGMENT WORD PUBLIC
INTRS ENDS
	;scan interrupt table for interrupt service routines
	MOV	AX, INTRS
	MOV	ES, AX
	MOV	SI, -4
_NXTINTR:
	ADD	SI, 4
	MOV	BX, ES:[SI]
	CMP	BX, 0
	JZ	_NXTINTR
	CMP	BX, 1A1AH
	JZ	_TASKCVT
	PUSH	SI
	PUSH	ES

	PUSH	BX
	MOV	AX, ES:2[SI]
	PUSH	AX
	MOV	CL, 2
	CALL	__ADDINTR
	ADD	SP, 4

	POP	ES
	POP	SI
	JMP	_NXTINTR
_TASKCVT:

	;GET DOS CRITICAL FLAG LOCATION          
	MOV      AH,34H
	INT      21H
	MOV      _CRITICAL,BX
	MOV      _CRITICAL+2,ES

	POP	ES
#endasm

	_Go();

}

/* initiate i/o operation and make requesting task wait in interrupt 
	condition variable. Return -1 in retcode if no such interrupt 
	number in interrupt list */
void StartIO(intrnum, fn, retcode)
int intrnum; int (*fn)(); int *retcode;
{
	int *entry;
	int errcode;

	disable();

	/* locate interrupt in interrupt descriptor list */
	for (entry = intr; entry != NULL; entry = entry[INT_NEXT])
		{
		if (entry[INT_NUM] == intrnum)
			break;
		}

	/* initiate i/o operation and wait */
	if (entry) {
		if (fn) {
			if ((errcode = (*fn)()) == 0) {
				Wait(&entry[INT_COND]);
				}
			else {
				entry[INT_ERROR] = errcode;
				}
			}
		else
			Wait(&entry[INT_COND]);

		*retcode = entry[INT_ERROR];
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
int prior;
{
	running[TD_PRIOR] = prior;
}

/* get a byte from input port */
inpbyte(port)
int port;
{
#asm
	MOV	DX, 4[BP]
	IN	AL, DX
	XOR	AH, AH
#endasm
}

/* write a byte to output port */
void outpbyte(port, value)
int port; int value;
{
#asm
	MOV	DX, 6[BP]
	MOV	AX, 4[BP]
	OUT	DX, AL
#endasm
}

/* disable interrupt */
void disable()
{
#asm
	CLI
#endasm
}


/* enable interrupt */
void enable()
{
#asm
	STI
#endasm
}


