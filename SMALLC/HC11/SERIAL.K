/**************************************/
/*       68HC11 SERIAL LIBRARY        */
/*             written by             */
/*            YU-SEOK KIM             */
/*           NEW MICROS,INC.          */
/**************************************/

/* Get char from serial channel */
ser_in()
{
#asm
EMPTY LDAB  SCSR
      BITB  #$20
      BEQ   EMPTY
      CLRA
      LDAB  SCDR
#endasm
}

/* Put char into serial channel */
ser_out(c)
char c;
{
#asm
FULL  LDAB  SCSR
      BITB  #$80
      BEQ   FULL
      LDAB  ARG1+1,Y
      STAB  SCDR
#endasm
}

/* Establish serial channel */
ser_setup()
{
#asm
      SEI
      LDAA   #$30
      STAA   BAUD
      LDAA   #0
      STAA   SCCR1
      LDAA   #$0C
      STAA   SCCR2
#endasm
}
