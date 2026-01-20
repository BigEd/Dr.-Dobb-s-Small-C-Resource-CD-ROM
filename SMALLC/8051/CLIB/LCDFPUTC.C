/* REQUIRES  syslib51.c */

/*
** Character-stream output of a character to fd.
** Entry: ch = Character to write.
**        fd = File descriptor of perinent file.
** Returns character written on success, else EOF.
** 8051: fd ignored.  Uses serial i/o routine _putkey.
    EOF case deleted; switch replaced by if/else.

** fd is switched via a global variable _OUT_ to
** either lcd or console

*/

int _OUT_ = 0;

/* REQUIRES  syslib51.c */

/*
** Character-stream output of a character to fd.
** Entry: ch = Character to write.
**        fd = File descriptor of perinent file.
** Returns character written on success, else EOF.
** 8051: fd ignored.  Uses serial i/o routine _putkey.
    EOF case deleted; switch replaced by if/else.
*/
fputc(ch, fd) int ch, fd; {
  if (_OUT_ == 0) {          /* stdio */
     if(ch=='\n') {
         _putkey(CR); _putkey(LF);
     }
     else {
         _putkey(ch);
     }
  }
  else {                /* lcdio */
     if(ch=='\n') {
        _lcdcr();
     }
     else {
        _lcdput(ch);
     }
     return (ch);
   }
}
#asm
.putc:  ljmp  .fputc
#endasm



_lcdcr() {
        char z;
        int i;
        _lcdbusy();
        z = peekb(0x0FFE0) & 0x040 ^ 0x0C0;
        pokeb(0x0FFE0,z);
        for (i=0;i++;i<40)
           _lcdput(' ');
        _lcdbusy();
        pokeb(0x0FFE0,z);
}

_lcdput(ch) char ch; {
        _lcdbusy();
        pokeb(0x0FFE1,ch);
}

_lcdinit() {
        _lcdbusy();
        pokeb(0x0FFE0,0x38);
        _lcdbusy();
        pokeb(0x0FFE0,0x38);
        _lcdbusy();
        pokeb(0x0FFE0,0x06);
        _lcdbusy();
        pokeb(0x0FFE0,0x0C);
}

_lcdbusy() {
        while(peekb(0xFFE0) & 0x80);
}

_lcdclear() {
        _lcdbusy();
        pokeb(0x0FFE0,0x01);
        _lcdbusy();
        pokeb(0x0FFE0,0x02);
}
