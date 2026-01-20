/* Get char from serial channel */
FOR_in()
{
#asm
EMPTY LDAB  $B02E
      BITB  #$20
      BEQ   EMPTY
      CLRA
      LDAB  $B02F
#endasm
}
