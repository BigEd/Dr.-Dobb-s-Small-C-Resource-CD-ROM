/*
** Write string to standard output. 
*/
puts(string) char *string; {
  while(*string) putchar(*string++) ;
  putchar('\n');
  }

