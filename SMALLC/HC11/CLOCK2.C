/**********************************************************************/
/* PROGRAM NAME: CLOCK2.C                                             */
/* PROGRAM TO DEMOSTRATE THE USE OF INTERRUPTS USING SMALL C.         */
/* PROGRAM WRITTEN FOR 68HC11 BOARD                                   */
/* PROGRAM SIZE: 4200 BYTES                                           */
/*  NOTE: fprintf() uses 3500 bytes of those 4200 bytes- instead use  */
/*        fputs() (about 120 bytes) and itoa() (about 100 bytes)      */
/*        if memory is tight.                                         */
/*    ENSURE THERE ARE AT AT LEAST 0x80 BYTES OF STACK SPACE!!!!      */
/**********************************************************************/

/* ROM version */

/* Define where things should reside... */
#code    0xE000
#data    0x0000
#stack   0xFF

/* Define where register bank is mapped... */
#include <init.k>

/* Almost always need these standard definitions... */
#include <68hc11.h>
#include <stdio.h>

/* Link in the following libraries (nearly all needed by fprintf() ) */
/* NOTE THE PATH (LIB\)--- YOU MAY NEED TO CHANGE THIS */

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

/* Link in the device driver for the 68HC11 */
#include <SER_out.c>

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
   bit_set(REG_BASE+TFLG2,0x80);
}

main()
{
/* Set Prescale Factor to 16 within 64 cycles */
#asm
      LDX   #$1024
      BSET  0,X $03
#endasm

   /* Set up Baud rate and Turn on SCI */
   pokeb(REG_BASE+BAUD,0x30);
   pokeb(REG_BASE+SCCR2,0x0C);

   /* jump to TOI vector */
/*   pokeb(0xD0,0x7E);
   poke(0xD1,toi);
*/

   /* Clear the Timer Overflow Flag */
   bit_set(REG_BASE+TFLG2,0x80);
 
   /*  Enable the Timer Overflow Interrupt */
   bit_set(REG_BASE+TMSK2,0x80);
 
   /* Enable interrupts */
   e_int();

   sys_clk=0;
   stdout=fopen(SER_out);
   fprintf(stdout,"Starting system timer...\n");
   while (1)
   {
      unsigned int temp;
      temp=sys_clk;
      while (temp/2==sys_clk/2);
      fprintf( stdout,message,sys_clk/120,(sys_clk/2) % 60);
   }
}
