/* +-------------------------------------------------------+ 
   |  Program Name: LCD_BUF.C                                  |
   |  Purpose: Program reads the keyboard or keypad input  |
   |           and displays (ASCII) characters on LCD.     |
   |  Program written for NMIX/T-0020 board.               |
   |  Note:  If you're using 5x4 keypad,  uncomment the    |
   |         the 'switch' statement in the keypad routine. |
   +-------------------------------------------------------+ */
/* BUFFALO version */

/* define where the program should reside */
#code 0xC100   /* assume RAM is installed on U3 socket */
#data 0xC000

#include <init.k>
#include <startup.c>
#include <68hc11.h>
#include <stdio.h>

/* libraries needed by fprintf() */
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

/* Link in the device driver for the 68hc11 */
#include <SER_in.c>
#include <SER_out.c>  

/* define LCD registers & ASCII control charaters */
#define LCD_CMD  0xB5FC     /* LCD Command Register */
#define LCD_DATA 0xB5FD     /* LCD Data Register */
#define ABORT    0x18       /* KEY [Ctrl+X] */
#define BS 0x08             /* ASCII Backspace */
#define K_BOARD  '1'
#define K_PAD    '2'
#define QUIT     '3'

FILE stdout; 

char c;
char str1[] = " Keyboard input test: ";
char str2[] = " Keypad input test: ";
char str3[] = " Have a nice day!";

/* check for busy flag */
not_busy()
{
   if((peekb(LCD_CMD) & 0x80) == 0 )
      return 1;
   else
      return 0;
}

/* initialized LCD */
dsp_on()
{
   not_busy();
   pokeb(LCD_CMD,0x38);  /* get attn */
   pokeb(LCD_CMD,0x38);  /* set 2 line disp */
   pokeb(LCD_CMD,0x06);  /* character entry right */
   pokeb(LCD_CMD,0x0E);  /* display control on, cursor on */
   pokeb(LCD_CMD,0x01);  /* set cursor home */
}

/* sending message to the LCD */
message(str)
char str[];
{
   unsigned int m;
   m = 0; 
   while(str[m] != '\0')
   {
     if(not_busy())
       pokeb(LCD_DATA,str[m++]);
   }
   return 0;
}

/* echos console's input on lcd */ 
keyboard()
{
    fputs("\n Echos keyboard's input on LCD...\n",stdout);
    fputs(" (Back to the main menu, press [CTRL+X])\n",stdout);
    while((c = SER_in()) != ABORT)
    {          
      if(not_busy())
      {
	 if (c == BS)      /* back space & delete character */
	 { 
	    pokeb(LCD_CMD,0x10);
	    pokeb(LCD_DATA,0x20);
	    pokeb(LCD_CMD,0x10);     
	 }
	 else if (c == CR)   /* advance cursor to the next line */
	    pokeb(LCD_CMD,0xC0); 
	 else
	    pokeb(LCD_DATA,c); /* put character on lcd */  
      }
    }  
    return(0);
}

/* routine for 4x5 kepad - reads the key strokes from the keypad, 
   converts them to ASCII characters, and displays on LCD. */
keypad()
{
      unsigned int k, period;
      period=500;        
      fputs("\n Ready for keys to be pressed from the Keypad...\n",stdout);   
      fputs(" (Back to the main menu, press 'J' button)\n",stdout);
      while(k != 'J')  /* press 'J' key to terminates keypad loop */
      {
	 while(peekb(REG_BASE+PORTA) & 0x01)  
	 {
	    k = peekb(REG_BASE+PORTE) >> 3;
	    {
  /* ===>   for 5x4 keypad, uncomment the switch statement below  */ 
	    /* --------------------------------------------------------    
	       switch(k)                                            
	       {                                                    
		  case 1  : k = 4;  break; case 4  : k = 1; break;  
		  case 2  : k = 8;  break; case 8  : k = 2; break;  
		  case 3  : k = 12; break; case 12 : k = 3; break;  
		  case 6  : k = 9;  break; case 9  : k = 6; break;  
		  case 7  : k = 13; break; case 13 : k = 7; break;  
		  case 11 : k = 14; break; case 14 : k = 11; break; 
		  default : break;                                  
	       }                                                    
	      -------------------------------------------------------- */    
	       if(k >= 9)
		  k+='7';
	       else
		  k+='0';
	    }
	    pokeb(LCD_DATA,k);
	    delay(period); 
	 }
      }
      return(0);
}

main()
{
 unsigned int loop;
 loop = 1;
 
 #asm           /* stack is top of the U3 ram  */
   LDS #$DFFF
 #endasm

 while(loop)  
 {
   stdout=fopen(SER_out);
   fputs("\n Please press a number to select a test below:",stdout);
   fputs("\n               [1] Keyboard",stdout);
   fputs("\n               [2] Keypad",stdout);
   fputs("\n               [3] Exit to FORTH\n",stdout);
   while( (c=SER_in()) && (c != K_BOARD && c != K_PAD && c != QUIT))
   {
    fprintf(stdout,"\n %c Invalid input, please try again!\n",BELL);
   }
   switch(c)
   {
      case K_BOARD :
	dsp_on();
	message(str1);
	keyboard();
	break;
      case K_PAD :
	dsp_on();
	message(str2);
	keypad(); 
	break;
      case QUIT :
	fputs("\n Have a nice day!\n",stdout);
	dsp_on();
	message(str3);
	loop = 0;
	break;
   }  
 }  
   return 0;
}
