/* Written by M. Taylor */


fputs(s,stream) char *s; FILE stream;{
 while (*s){
   fputc(*s++,stream);}
}
