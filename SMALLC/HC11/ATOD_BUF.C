/***********************************************************************/
/* Program Name: ATOD_BUF.C                                            */
/* Program demonstrates A/D Conversion with NMI 68HC11 board running   */
/*         BUFFALO monitor.                                            */   
/* Program displays A/D converted value on terminal every 1 second     */
/* NOTE: Ensure there are atleast 0x80 bytes of stack space, and       */
/*       8K ram must installed on U3 socket (0xC000 - 0xDFFF).         */
/***********************************************************************/

/* BUFFALO version */

/* Define where things should reside... */
#code    0xC0F0
#data    0xC000

/* Define where register bank is mapped... */
#include <startup.c>
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
rom char message[]=" %5u %5u %5u %5u";

main()
{
   unsigned int period;
#asm
   LDS  #$DFFF
#endasm
   period=1000;
   stdout=fopen(SER_out);
   header();
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

header()
{
   int i;
   fprintf(stdout,"                  CHANNEL READINGS \n");
   for(i=1;i<=9;i++)
   { 
      fprintf(stdout,"%6u",i);
   }
   fprintf(stdout,"\n   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}
