#include <stdio.h>

static char msg[128];	/* message to send */

/* xmitter task */
void task xmitter(256)
{
	while (1) {
		/* get message and send over serial port */
		GetMsg(msg);
		puts(msg);
		}	
}

