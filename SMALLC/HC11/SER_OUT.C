SER_out(ch)
char ch;
{
#asm
FULL  LDAB  $102E
      BITB  #$80
      BEQ   FULL
      LDAB  7,Y
      STAB  $102F
#endasm
}
