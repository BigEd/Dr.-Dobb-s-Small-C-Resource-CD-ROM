/***********************************************************************/
/* Program Name: ATOD.C                                                */
/* Program to demonstrate A/D Conversion with 68HC11 board             */
/* Program displays A/D converted value on terminal every 1 second     */
/* Programmer: Yu-Seok Kim, New Micros,Inc.                            */
/* Date : March 2,1993                                                 */
/* Note: 8k ram must installed on U2 socket (memory location 0 to 1FFF */
/***********************************************************************/

/* FORTH version */

/* Define where things should reside... */
#code    0x0800
#data    0x0700

/* Define where register bank is mapped... */
#include <forth.k>

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
#include <FOR_out.c>

FILE stdout;

/* Make the message live in ROM, so it's there on power up!! */
rom char message[]="%u %u %u %u ";

main()
{
   unsigned int period;

   period=1000;
   stdout=fopen(FOR_out);
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
