/*
** CSYSLIB -- System-Level Library Functions
*/

/*
** Test for keyboard (serial port) input
*/
_hitkey() {
#asm
        mov a,scon      ; get rx flag in bit 0
        anl a,#1
        mov r0,a
        mov r4,#0       ; pr=true if rx char available
#endasm
  }

/*
** Return next keyboard (serial port) character
*/
_getkey() {
#asm
        jnb scon.0,*    ; await Rx interrupt flag
        clr scon.0
        mov r0,sbuf     ; get new char in pr
        mov r4,#0
#endasm
  }

/*
** Output character to serial port
*/
_putkey(c) unsigned char c; {
        c;              /* get char in pr */
#asm
        jnb scon.1,*    ; await Tx interrupt flag
        clr scon.1      ; clear flag
        mov sbuf,r0     ; output pr char to UART
#endasm
  }


