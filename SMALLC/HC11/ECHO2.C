/* Echo characters to terminal */
/* ROM version                 */

#data 0x0000			/* Data segment is at $0000 	   */
#code 0xE000			/* Code segment is at $E000      */
#stack 0xFF			/* Base Address of Stack is $100 */
#include <init.k>
#include <68hc11.h>
#include <ser_in.c>
#include <ser_out.c>

char c;				/* Declare char variable c       */

main()				/* Begin of main routine         */
{
						
   pokeb(REG_BASE+BAUD,0x30); /* Set up the serial channel     */
   pokeb(REG_BASE+SCCR2,0x0C);

      while(1)			/* Create an infinite loop       */ 
   {
      c=SER_in();		/* Read a char from serial       */
      SER_out(c);		/* Write a char to serial        */
   }
}				/* End of main routine           */

