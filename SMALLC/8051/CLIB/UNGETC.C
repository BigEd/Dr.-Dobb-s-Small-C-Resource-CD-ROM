/* REQUIRES  fgetc.c */

/*
** Put c back into file fd.
** Entry:  c = character to put back
**        fd = file descriptor
** Returns c if successful, else EOF.
** 8051: fd ignored; _nextc is a scalar.  Deleted reference to
    _mode().
*/
ungetc(c, fd) int c, fd; {
  if(_nextc!=EOF || c==EOF) return (EOF);
  return (_nextc = c);
  }

