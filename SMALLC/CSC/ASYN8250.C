#include <stdio.h>

/* 8259 interrupt controller interrupt mask register port# */
#define MASK8259	0x21

/* async 8250 constants */
/* these parameters are for com2 */
#define IOBASE		0x2F0
#define INTVEC		0x0B
#define DEVMASKBIT	0x08

/* 8250 control register port number */
#define RLINECTR	(IOBASE + 0x0B)
#define RLBAUD		(IOBASE + 0x08)
#define RHBAUD		(IOBASE + 0x09)
#define RLINESTAT	(IOBASE + 0x0D)
#define RRECV		(IOBASE + 0x08)
#define RXMIT		(IOBASE + 0x08)
#define RINTEN		(IOBASE + 0x09)
#define RMODEMC		(IOBASE + 0x0C)
#define RINTID		(IOBASE + 0x0A)
#define RMODEMSTAT	(IOBASE + 0x0E)

#define TXREADY		0x20	/* ready to transmit */
#define SETDIVISOR	0x80	/* set divisor */
#define L2400		0x30	/* low value for clock divisor */
#define H2400		0x00	/* high value for clock divisor */
#define DATA8		0x03	/* 8-bit data */
#define DCD		0x80	/* data carrier detect */
#define DSR		0x20	/* data set ready */
#define CTS		0x10	/* clear to send */
#define DTR		0x01	/* data terminal ready */
#define RTS		0x02	/* request to send */
#define OUT2		0x08	/* output 2 signal */
#define TXEMPTY		0x02	/* Tx holding register empty interrupt */

#define COMPLETED	0xFF	/* user-defined completion return code */

static int pos;		/* position of char in msg to send */
static char msg[128];	/* message to send */
static int linefeed;	/* time to send a linefeed */

/* send a char over the asyn link */
sendchar(ch)
char ch;
{
	/* check for modem status */
	if (inpbyte(RMODEMSTAT) & (DCD | DSR | CTS)) {
		/* wait till Tx is ready and send */
		while ((inpbyte(RLINESTAT) & TXREADY) != TXREADY);
		outpbyte(RXMIT, ch);
		return 0;
		}

	return 1;	
}

/* initialize the 8250 chip */
void Init8250()
{
	/* set baud rate at 2400, no parity, 8-bit data */
	outpbyte(RLINECTR, SETDIVISOR);
	outpbyte(RLBAUD, L2400);
	outpbyte(RHBAUD, H2400);
	outpbyte(RLINECTR, DATA8);

	/* set modem control */
	outpbyte(RMODEMC, (DTR | RTS | OUT2));

	/* enable 8250 Tx holding register empty interrupt */
	outpbyte(RINTEN, TXEMPTY);

	/* enable 8259 interrupt controller for com2 */
	outpbyte(MASK8259, inpbyte(MASK8259) & ~DEVMASKBIT);
}

/* interrupt service routine to send a message. Note: a non-zero
	return value wakes up the xmitter task which initiated
	the transmission by calling StartIO */
interrupt asynsend(INTVEC)
{
	char ch;
	int retcode;

	if (linefeed) {
		/* time to send a linefeed */
		linefeed = 0;
		retcode = sendchar(LF);
		}
	else if ((ch = msg[++pos]) != 0) {
		/* translate LF to CR followed by LF */
		if (ch == LF)
			{
			retcode = sendchar(CR);
			linefeed = 1;
			}
		else
			/* send char as is */
			retcode = sendchar(ch);
		}
	else
		return COMPLETED;

	return retcode;
}

/* start interrupt-driven message transmission */
startx()
{
	pos = 0;
	return sendchar(msg[0]);
}

/* xmitter task declaration: note use of high task priority */
void task xmitter(256)
{
	int status;

	Init8250();	/* init 8250 */

	while (1) {
		/* get message and print it */
		GetMsg(msg);
		printf(msg);

		/* start i/o operation to send message to remote console. 
			Only return when operation is completed or failed */
		StartIO(INTVEC, startx, &status);
		if (status != COMPLETED)
			printf("Async transmit error: %d\n", status);
		}	
}

