/* Echo characters to terminal */
/* FORTH version               */
/* NOTE: 8K ram must install on U2 socket (memory location: 0 to 1FFF */

#data 0x0700			/* Data segment is at $0700 	   */
#code 0x0800			/* Code segment is at $0800      */
		
#include <FORTH.k>
#include <FOR_in.c>
#include <FOR_out.c>

char c;				/* Declare char variable c       */

main()				/* Begin of main routine         */
{						
   while(1)			/* Create an infinite loop       */ 
   {
      c=FOR_in();		/* Read a char from the keyboard    */
      FOR_out(c);		/* Write a char to the screen       */
   }
}				/* End of main routine           */
