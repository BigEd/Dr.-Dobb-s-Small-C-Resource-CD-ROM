/*
** Test for keyboard (serial port) input
*/
_hitpad() {
#asm
        mov a,p1        ; get rx flag in bit 0
        anl a,#1
        mov r0,a
        mov r4,#0       ; pr=true if rx char available
#endasm
  }

_getpad() {
#asm
        jnb p1.0,*      ; await keypress interrupt flag
        mov a,0e8H      ; get new char in pr
        anl a,#01fh
        mov r0,a
        mov r4,#0
#endasm
  }


