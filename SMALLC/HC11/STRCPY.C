

/* Taken from J.E. Hendrix' Small C Compiler V2.2
** copy t to s
*/
strcpy(s, t) char *s, *t; {
  char *d;
  d = s;
  while (*s++ = *t++) ;
  return (d);
  }


