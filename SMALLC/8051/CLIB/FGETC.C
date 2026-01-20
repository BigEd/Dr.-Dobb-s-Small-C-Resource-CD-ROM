/* REQUIRES  syslib51.c */

/*
** Character-stream input of one character from fd.
** Entry: fd = File descriptor of pertinent file.
** Returns the next character on success, else EOF.
** 8051: Ignores fd.  Always uses serial port _getkey() and _putkey()
    routines.  Uses scalar _nextc.  Single switch statement for all
    cooking; DOSEOF and ABORT cases deleted.
*/

int _nextc = EOF;     /* for ungetc() */

fgetc(fd) int fd; {
  int ch;                   /* must be int so EOF will flow through */
  if(_nextc != EOF) {   /* an ungotten byte pending? */
    ch = _nextc;
    _nextc = EOF;
    return (ch & 255);      /* was cooked the first time */
    }
  while(1) {
    ch = _getkey();
    switch(ch) {          /* normal & console cooking */
        /* case ABORT:  exit(2); */
        case    CR: _putkey(CR); _putkey(LF); return('\n');
        case   DEL:  ch = RUB;
        case   RUB:
        case  WIPE:  return(ch);
        default:    _putkey(ch);  return(ch);
        case    LF: _putkey(ch);
      }
    }
  }
#asm
.getc:  ljmp .fgetc
#endasm

