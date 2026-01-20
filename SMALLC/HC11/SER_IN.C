SER_in()
{
#asm
EMPTY LDAB  $102E
      BITB  #$20
      BEQ   EMPTY
      CLRA
      LDAB  $102F
#endasm
}
