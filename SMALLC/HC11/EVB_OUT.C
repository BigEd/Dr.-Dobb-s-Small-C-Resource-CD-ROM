/* I/O Device Driver for Motorola's 68HC11 EVB */
/* Written by M. Taylor */


EVB_out(ch)char ch;{
 ch=ch;
#asm
      LDD 6,Y
      TBA
      JSR $FFB8
#endasm
}

