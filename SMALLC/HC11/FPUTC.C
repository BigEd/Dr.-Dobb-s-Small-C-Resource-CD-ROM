/* Written by M. Taylor */

fputc(ch,stream) int ch; FILE stream;{
 switch (ch) {
  case '\n':stream(0x0D); stream(0x0A); break;
  default  :stream(ch);
 }
 return(ch);
}
