/***********************************************************************/
/* Program Name: ATOD_ROM.C                                            */
/* Program to demonstrate A/D Conversion with 68HC11 board             */
/* Program displays A/D converted value on terminal every 1 second     */
/* Date : June 13, 1995.                                               */
/***********************************************************************/

/* ROM version */

/* Define where things should reside... */
#code    0xE000
#data    0x0000
#stack   0x01FF

/* Define where register bank is mapped... */
#include <init.k>

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
#include <delay.c>

/* Link in the device driver for the 68HC11 */
#include <SER_out.c>

FILE stdout;

/* Make the message live in ROM, so it's there on power up!! */
rom char message[]="%u %u %u %u ";

main()
{
   unsigned int period;

   /* Set up Baud rate and Turn on SCI */
   pokeb(REG_BASE+BAUD,0x30);
   pokeb(REG_BASE+SCCR2,0x0C);

   /* turn on AD coverter bit */
   pokeb(REG_BASE+OPTION,0x80);

   period=1000;
   stdout=fopen(SER_out);
   fprintf(stdout,"\nStarting A/D conversion...\n");

   while(1)
   {
      rd_anlg();
      delay(period); /* delays 1000 millisecond (= 1 second) */
   }
}

rd_anlg()
{
   pokeb(REG_BASE+ADCTL,0x10);
   fprintf(stdout,message,peekb(REG_BASE+ADR1),peekb(REG_BASE+ADR2),
                          peekb(REG_BASE+ADR3),peekb(REG_BASE+ADR4));
   pokeb(REG_BASE+ADCTL,0x14);
   fprintf(stdout,message,peekb(REG_BASE+ADR1),peekb(REG_BASE+ADR2),
                          peekb(REG_BASE+ADR3),peekb(REG_BASE+ADR4));

   fprintf(stdout,"\n");
}
