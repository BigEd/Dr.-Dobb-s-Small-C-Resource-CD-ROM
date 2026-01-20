/* 
   Checks for currently available keystrokes.
   kbhit returns a nonzero value. Otherwise,
   it returns 0.
   Written by C.N./NEW MICROS, INC.
*/

kbhit()
{
   unsigned int non_zero;
   if(peekb(REG_BASE+SCSR) & 0x20)
   {
     non_zero = peekb(REG_BASE+SCDR);
     return non_zero;
   }
   else
     return 0;
}

