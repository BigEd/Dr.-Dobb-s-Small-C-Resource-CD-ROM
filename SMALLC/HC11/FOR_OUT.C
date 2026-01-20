FOR_out(ch)
char ch;
{
#asm
FULL  LDAB  $B02E
      BITB  #$80
      BEQ   FULL
      LDAB  7,Y
      STAB  $B02F
#endasm
}