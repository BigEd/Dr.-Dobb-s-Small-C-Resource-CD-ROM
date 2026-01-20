/*******************************************************************/
/* Causes ms millisecond delay running on 68HC11 with a 8MHz x-tal */
/*             Written by Y. Kim, New Micros,Inc.                  */
/*******************************************************************/

delay(ms)
unsigned int ms;
{
   unsigned int i,j;

   j=peek(REG_BASE+TCNT);
   j+=5000;
   poke(REG_BASE+TOC5,j);
   pokeb(REG_BASE+TFLG1,0x08);
    
   for(i=0;i<=ms;i++)
   {
      while( !(peekb(REG_BASE+TFLG1) & 0x08) );
      j=peek(REG_BASE+TOC5);
      j+=2000;
      poke(REG_BASE+TOC5,j);
      pokeb(REG_BASE+TFLG1,0x08);
   }
}
