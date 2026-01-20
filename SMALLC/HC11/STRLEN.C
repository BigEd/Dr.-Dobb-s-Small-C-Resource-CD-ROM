/* written by M. Taylor */
strlen(s) char *s; {
 int i;
 i=0;
 while (*s++) i++;
 return i;
}
