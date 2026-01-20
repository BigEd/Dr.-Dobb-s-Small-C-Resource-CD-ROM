/***********************************************************************/
/* Program Name: ATOD2.C                                                */
/* Program demonstrates the A/D Conversion with NMI 68HC11 board,      */
/* Program displays A/D converted value on terminal every 1 second     */
/* Programmer: Yu-Seok Kim, New Micros,Inc. Modified by Chris Nguyen   */
/* Date : March 2,1993. Revised 4/25/94.                               */
/* Note : 8K ram must installed on U2 socket; or you may change the    */
/*        #code, and #data to the appropriate memory location.         */
/***********************************************************************/

/* FORTH version */

/* Define where things should reside... */
#code    0x800
#data    0x700

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
#include <FOR_in.c>

FILE stdout;

/* Make the message live in ROM, so it's there on power up!! */
rom char message[]="%2u.%2u ";

main()
{
   unsigned int period;
   period=1000;
   stdout=fopen(FOR_out);
   fprintf(stdout,"NOTE: Input Voltage: Max = VRH(+5v), Min = VRL(0v). \n");
   fprintf(stdout," Results will be displayed in decimal values, and all \n");
   fprintf(stdout," the blanks after decimal points count as Zeros(0).\n");
   fprintf(stdout," Reading accuracy: +/- 0.05v \n");
   pause(); 
   fprintf(stdout,"\nStarting A/D conversion...\n");
   fprintf(stdout,"                 CHANNEL READINGS \n");
   fprintf(stdout,"  PE0   PE1   PE2   PE3   PE4   PE5   PE6   PE7 \n");
   fprintf(stdout,"  ~~~   ~~~   ~~~   ~~~   ~~~   ~~~   ~~~   ~~~ \n");
   while(1)
   {
      pokeb(REG_BASE+ADCTL,0x10); /* set A/D control register to   */
      rd_anlg();                  /* read the 1st four channels.   */
      pokeb(REG_BASE+ADCTL,0x14); /* set A/D control register to   */
      rd_anlg();                  /* reads channel 5 to channel 8. */
      fprintf(stdout,"\n");
      delay(period);              /* delays 1 second */
   }
}

/* reads the inputs, and convert to decimal, and display the values */
rd_anlg()       
{
   unsigned int i, j, arr[4];
   j=1;
   for(i=0;i<=4;i++)
   { 
     arr[i]=peekb(REG_BASE+ADCTL+j);
     fprintf(stdout,message,arr[i]/51,((arr[i]%51)*100)/51);
     j++;
   }
}

/* check if ENTER key is hit?  */
pause()
{
  unsigned int c;
  fprintf(stdout," Press ENTER to continue...\n");
  c=FOR_in();
  while(c != 0x0D)
  {
     c=FOR_in();
  }
}
 