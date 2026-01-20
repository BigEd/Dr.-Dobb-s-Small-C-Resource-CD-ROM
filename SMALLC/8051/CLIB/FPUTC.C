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
  if(ch=='\n') {
      _putkey(CR); _putkey(LF);
  }
  else {
      _putkey(ch);
  }
  return (ch);
  }
#asm
.putc:  ljmp  .fputc
#endasm

