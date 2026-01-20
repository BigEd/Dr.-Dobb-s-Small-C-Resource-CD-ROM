/**********************************************************************/
/* PROGRAM NAME: CLOCK.C                                              */
/* PROGRAM TO DEMOSTRATE THE USE OF INTERRUPTS USING SMALL C.         */
/* PROGRAM WRITTEN FOR MOTOROLA 68HC11 BOARD RUNNING BUFFALO MONITOR. */
/* PROGRAM SIZE: 4200 BYTES                                           */
/*  NOTE: fprintf() uses 3500 bytes of those 4200 bytes- instead use  */
/*        fputs() (about 120 bytes) and itoa() (about 100 bytes)      */
/*        if memory is tight.                                         */
/*    ENSURE THERE ARE AT AT LEAST 0x80 BYTES OF STACK SPACE!!!!      */
/**********************************************************************/

/* Define where things should reside... */
#code    0xC0F0
#data    0xC000
#include <startup.c>

/* Define where register bank is mapped... */
#define  REG_BASE 0x1000

#include <68hc11.h>
#include <stdio.h>

/* Link in the following libraries (nearly all needed by fprintf() )*/

#include <fopen.c>
#include <fputs.c>
#include <fputc.c>
#include <itoa.c>
#include <atoi.c>
#include <is.c>
#include <fprintf.c>
#include <itoab.c>
#include <reverse.c>
#include <strlen.c>

/* Link in the device driver for the Motorola 68HC11 EVB */

#include <EVB_out.c>

FILE            stdout;
unsigned int    sys_clk;
char            buf[10];

/* Make the message live in ROM, so it's there on power up!! */
rom char        message[]="ELAPSED TIME: %u minutes and %u seconds.\n";

/* Define the ISR routine... */
/* Called whenever the system timer overflows (about every 0.5 seconds) */

interrupt toi()
{
   sys_clk++;
   /* Reset the timer overflow interrupt flag so it can happen again */
   bit_set(0x1025,0x80);
}

main()
{
/* Set the PreScale bits before the time protect timer expires */
#asm
      LDX  #$1024
      LDAB #$03
      STAB 0,X
      LDS  #$DFFF
#endasm
   /* change the Timer Overflow Interrupt Vector to point to our routine */
   poke(0xD1,toi);
   /* Clear the Timer Overflow Flag */
   bit_set(REG_BASE+TFLG2,0x80);
   /*  Enable the Timer Overflow Interrupt */
   bit_set(REG_BASE+TMSK2,0x80);
   /* Enable interrupts */
   e_int();
   sys_clk=0;
   stdout=fopen(EVB_out);
   fprintf(stdout,"Starting system timer...\n");
   while (1)
   {
      unsigned int temp;
      temp=sys_clk;
      while (temp/2==sys_clk/2);
      fprintf( stdout,message,sys_clk/120,(sys_clk/2) % 60);
   }
}
