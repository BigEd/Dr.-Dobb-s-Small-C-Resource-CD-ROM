#include <stdio.h>

/* monitor data */
static CONDITION conready;	/* console ready condition variable */
static CONDITION msgavail;	/* message available condition variable */
static int busy;		/* buffer inuse invariant */
static char buffer[128];	/* monitor's message buffer */

/* console monitor initialization */
void monitor mconsole()
{
	printf("Initializing console monitor...\n");
}

/* monitor entry to send message to console */
void entry Send(msg)
char *msg;
{
	/* wait till buffer is not in use */
	if (busy) 
		Wait(&conready);

	/* copy message to local buffer and wake up console */
	strcpy(buffer, msg);
	busy = 1;
	Signal(&msgavail);
}

/* monitor entry for console to get message */
entry GetMsg(msg)
char *msg;
{
	/* wait till message is available */
	if (!busy)
		Wait(&msgavail);

	/* copy message to caller's buffer and wake up any task 
		waiting to deliver a message */
	strcpy(msg, buffer);
	busy = 0;
	Signal(&conready);
}

