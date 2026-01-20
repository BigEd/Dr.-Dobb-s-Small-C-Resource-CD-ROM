/**********************************************************************/
/* PROGRAM NAME: CLOCK4TH.C                                           */
/* PROGRAM DEMONSTRATES THE USE OF TIMER OVERFLOW INTERRUPTS WRITTEN  */
/*         IN SMALL C RUNNING UNDER FORTH.                            */
/* PROGRAM WRITTEN FOR NEW MICROS F68HC11 BOARDS.                     */      
/* MODIFIED FROM CLOCK1.C BY CHRIS NGUYEN (9/25/94).                  */
/**********************************************************************/

/* FORTH VERSION */

/* Define where things should reside... */
#code    0x0800  /* must installed ram on U2 socket,             */
#data    0x0700  /* else change #code & #data to the appropriate */
                 /* ram locations.                               */

/* include FORTH utilities & HC11 registers  */
#include <forth.k>
#include <68hc11.h>
#include <stdio.h>

/* Link in libraries (most needed by fprintf()) */
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
#include <progee11.c> 
#include <FOR_out.c>        

FILE            stdout; 
unsigned int    sys_clk;
char            buf[10];

/* Make the message live in ROM, so it's there on power up!! */
   rom char  message[]="ELAPSED TIME: %u minutes and %u seconds. \n"; 

/* Define the ISR routine... */

/* Called whenever the system timer overflows (about every second) */
interrupt toi()
{
   sys_clk++;
   /* Reset the timer overflow interrupt flag so it can happen again */
   bit_set(REG_BASE+TFLG2,0x80);
}

main()
{
/* change the Timer Overflow Interrupt Vector to point to our routine */
   eepokeb(0xB7CB,0x7E);               /* jump to TOI vector */
   eepokeb(0xB7CC,(toi & 0xFF00)>>8);  /* write TOI high byte */
   eepokeb(0xB7CD,(toi & 0x00FF));     /* write TOI low byte */

   /* Clear the Timer Overflow Flag */
   bit_set(REG_BASE+TFLG2,0x80);
   
   /*  Enable the Timer Overflow Interrupt */
   bit_set(REG_BASE+TMSK2,0x80);
   
   /* Enable interrupts */
   e_int();

   sys_clk=0;
   stdout=fopen(FOR_out);
   fprintf(stdout,"Starting system timer...\n");
   while (1)
   {
      unsigned int temp;
      temp=sys_clk;
      while (temp/2/16==sys_clk/2/16);
      fprintf(stdout,message,sys_clk/120/16,(sys_clk/2/16) % 60);
   }
}
