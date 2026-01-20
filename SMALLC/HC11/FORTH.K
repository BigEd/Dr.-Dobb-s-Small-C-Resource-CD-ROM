/******************/
/*    FORTH.K     */
/******************/

#define REG_BASE 0xB000

#asm
AUTOSTRT EQU   $A55A
         FDB   AUTOSTRT
         FDB   _cfa
_cfa     FDB   _main
#endasm
