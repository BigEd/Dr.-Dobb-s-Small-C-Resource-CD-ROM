/* +-------------------------------------------------------+ 
   |  Program Name: LCD.C                                  |
   |  Purpose: Program reads the keyboard or keypad input  |
   |           and displays (ASCII) characters on LCD.     |
   |  Program written for NMIX/T-0020 board.               |
   |  Note:  If you're using 5x4 keypad,  uncomment the    |
   |         the 'switch' statement in the keypad routine. |
   +-------------------------------------------------------+ */
/* FORTH version */

/* define where the program should reside */
#code 0x0800   /* make sure RAM is installed on U2 socket */
#data 0x0700

#include <forth.k>
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
#include <FOR_in.c>
#include <FOR_out.c>  

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
busy()
{
   if(peekb(LCD_CMD) & 0x80) 
      return 0;
   else
      return 1;
}

/* initialized LCD */
dsp_on()
{
   while(! busy());
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
     while(! busy());
       pokeb(LCD_DATA,str[m++]);
   }
   return 0;
}

/* echos console's input on lcd */ 
keyboard()
{
    fputs("\n Echos keyboard's input on LCD...\n",stdout);
    fputs(" (Back to the main menu, press [CTRL+X])\n",stdout);
    while((c = FOR_in()) != ABORT)
    {          
      while(! busy());
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
	 while(peekb(REG_BASE+PORTA) & 0x01) /* chek for keypad interrupt */  
	 {
	    k = peekb(REG_BASE+PORTE) >> 3; /* gets data from PortE */
	    {
	       switch(k)    /* offset data to corresponding key numbers */                                          
	       {      
		     /*** for 4x5 keypad, part # 86JB2-202 ***/                                        
		  case 0  : k = 3;  break; case 1  : k = 2;  break;  
		  case 2  : k = 1;  break; case 3  : k = 0;  break;  
		  case 4  : k = 7;  break; case 5  : k = 6;  break;  
		  case 6  : k = 5;  break; case 7  : k = 4;  break;  
		  case 8  : k = 11; break; case 9  : k = 10; break;  
		  case 10 : k = 9;  break; case 11 : k = 8;  break; 
		  case 12 : k = 15; break; case 13 : k = 14; break;
		  case 14 : k = 13; break; case 15 : k = 12; break;
		  case 16 : k = 19; break; case 17 : k = 18; break;
		  case 18 : k = 17; break; case 19 : k = 16; break;
						
		     /*** for 5x4 keypad, part # 86JB2-203 ***/                                        
	       /* -------------------------------------------------
		  case 1  : k = 4;  break; case 4  : k = 1;  break;  
		  case 2  : k = 8;  break; case 8  : k = 2;  break;  
		  case 3  : k = 12; break; case 12 : k = 3;  break;  
		  case 6  : k = 9;  break; case 9  : k = 6;  break;  
		  case 7  : k = 13; break; case 13 : k = 7;  break;  
		  case 11 : k = 14; break; case 14 : k = 11; break; 
		  ------------------------------------------------- */   
		  default : break;
	       }                                                    
	       (k >= 9)? (k+='7') : (k+='0');  /* converts to ascii */
	    }
	    pokeb(LCD_DATA,k);    /* displays on LCD */
	    delay(period);        /* wait for key debounce */
	 }
      }
      return(0);
}

main()
{
 unsigned int loop;
 loop = 1;
 while(loop)  
 {
   stdout=fopen(FOR_out);
   fputs("\n Please press a number to select a test below:\n",stdout);
   fputs("      [1] Keyboard\n      [2] Keypad\n      [3] Exit to FORTH\n",stdout);
   c=FOR_in();
   while(c != K_BOARD && c != K_PAD && c != QUIT)
   {
    fprintf(stdout,"\n %c Invalid input, please try again!\n",BELL);
    c=FOR_in();
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
