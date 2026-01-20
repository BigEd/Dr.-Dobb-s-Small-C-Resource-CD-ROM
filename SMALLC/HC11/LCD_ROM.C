
/* +--------------------------------------------------------------+ 
   |  Program Name: LCD_ROM.C                                     |
   |  Purpose: Displays character strings & keypad data on LCD,   |
   |           using NMIX/T-0020 board interfaced with LCD(2x40). |
   |  Written by: Chris Nguyen @ NEW MICROS, INC.                 |
   |  Note: This is an example of Rom stand alone version, it is  |
   |        ready to compile and burn the eprom.                  |
   +--------------------------------------------------------------+ */
                                                                             
/* --- define where things should reside --- */
#code  0xE000   /* Code mapped for U4 socket. */
#data  0x0000   /* Use internal ram for data & stack, */
#stack 0x01FF   /* external ram is not required for this program. */

/* --- ROM version --- */
#include <init.k>

/* included libraries  */
#include <68hc11.h>
#include <stdio.h>
#include <itoa.c>
#include <atoi.c>
#include <is.c>
#include <reverse.c>
#include <strlen.c>

/* define LCD registers  */
#define LCD_CMD  0xB5FC     /* LCD Command Register */
#define LCD_DATA 0xB5FD     /* LCD Data Register */

/* reserve rom buffer for character strings   */
rom char str1[] = "HELLO WORLD!";
rom char str2[] = "Press on any button:";

/******************************* LCD ROUTINES ******************************/

/* --- check for lcd busy flag --- */
busy()
{
   if (peekb(LCD_CMD) & 0x80)
      return 1;
   else
      return 0;
}

/* --- write to lcd_command register --- */
write_cmd(VALUE)
unsigned char VALUE;
{
    while(busy());
      pokeb(LCD_CMD,VALUE);
}

/* --- write to lcd_data register --- */
write_data(DATA)
unsigned char DATA;
{
   while(busy());
     pokeb(LCD_DATA,DATA);
}

/* --- display at position & shift right: dsp_at(row,column) --- */
dsp_at(ROW,COLUMN)
unsigned int ROW,COLUMN;
{
   unsigned int col;
   if(ROW == 1) 
      write_cmd(0x02);
   else
      write_cmd(0xC0);
   for(col = 0; col <= COLUMN-1; col++)
      write_cmd(0x14); 
}

/* --- put a character on lcd: lcd_putc('c') --- */
lcd_putc(c)
char c;
{
    while(busy());
      write_data(c);
}

/* --- put a string on lcd: lcd_puts(string) --- */
lcd_puts(s)
char s[];
{
   unsigned int i;
   i = 0; 
   while(s[i] != '\0')
     lcd_putc(s[i++]);
}

/* --- centers a string on 2x40 display: center_str(row,string) --- */
center_str(r,s)
unsigned int r;
char s[];
{
   unsigned int mid;
   mid = (21 - (strlen(s)/2));
   dsp_at(r,mid);
   lcd_puts(s);
}

/* --- initializes 2x40 lcd --- */
dsp_init()
{
   write_cmd(0x38); /* 8 bits, 2 lines lcd             */ 
   write_cmd(0x38); /* 8 bits, 2 lines lcd             */
   write_cmd(0x06); /* cursor shift, increment counter */
   write_cmd(0x0C); /* display on, cursor off          */
   write_cmd(0x01); /* clear display, set cursor home  */
}

/*************************** KEYPAD ROUTINES *******************************/

/* --- offset data for GrayHill 4x5 keypad part# 86JB2-203 --- */
rom int k_str[20] = {0,4,8,12,1,5,9,13,2,6,10,14,3,7,11,15,16,17,18,19}; 
                   /*   +----------------------+
                        |  [0] [1] [2] [3] [G] |
                        |                      |
                        |  [4] [5] [6] [7] [H] |   
                        |                      |
                        |  [8] [9] [A] [B] [I] | 
                        |                      |
                        |  [C] [D] [E] [F] [J] | 
                        +----------------------+  */
 
/* return nonzero, if keypad is pressed */
key_strobe()
{
   if(peekb(REG_BASE+PORTA) & 1)
   {
      while(peekb(REG_BASE+PORTA) & 1);   /* wait for key debounce */
      return 1;
   }
   else
     return 0;
}


/* --- get data from PE3-PE7, return integers: (0 to 19) --- */
kp_data()
{
    unsigned int k;
    k = peekb(REG_BASE+PORTE) >> 3;
    return k;
}

/* clear string strclr(row,column,string) */
strclr(r,c,s)      
int r,c;
char s[];
{
      int i;
      dsp_at(r,c);
      for(i = 0; i < strlen(s); i++)
         write_data(0x20);
      dsp_at(r,c);
}

/* read keypad key, convert to ascii, and put on lcd */
keypad(kstr)
int kstr[];
{
   unsigned int *count, i, m, n;
   char s1[6];
   count = 0x0100;
   i = 0;
   while(i < 32767)
   {
      *count = i++;
      dsp_at(2,2);
      lcd_puts(itoa(*count,s1));
      while(key_strobe())  
      {
         m = kp_data();
         (kstr[m] > 10) ? (n = kstr[m] + 55) : (n = kstr[m] + 48);
         dsp_at(2,31);
         write_data(n);
      }
   }
   strclr(2,2,s1);
}

/**************************** MAIN PROGRAM *********************************/
main()
{
   dsp_init();
   center_str(1,str1);
   dsp_at(2,10);
   lcd_puts(str2);
   keypad(k_str);  
}


