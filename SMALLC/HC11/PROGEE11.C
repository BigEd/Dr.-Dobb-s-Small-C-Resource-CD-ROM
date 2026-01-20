/* delay 10 millisecond */
delay_10ms()
{
#asm
     PSHX
     LDX   #$0D06
DLY  DEX   
     BNE   DLY
     PULX
#endasm
}

/* erase single byte of HC11 EEPROM */
eraseEEb(addr)
unsigned int addr;
{
   pokeb(REG_BASE+PPROG,0x16);
   pokeb(addr,0xFF);
   pokeb(REG_BASE+PPROG,0x17);
   delay_10ms();
   pokeb(REG_BASE+PPROG,0x00);
}

/* program a byte of HC11 EEPROM */
prog_eeb(addr,val)
unsigned int addr;
unsigned char val;
{
   pokeb(REG_BASE+PPROG,0x02);
   pokeb(addr,val);
   pokeb(REG_BASE+PPROG,0x03);
   delay_10ms();
   pokeb(REG_BASE+PPROG,0x00);
}
  
/* erase then program a single byte of HC11 EEPROM */
eepokeb(addr,val)
unsigned int addr;
unsigned char val;
{
   if(peekb(addr) != 0xFF)
     eraseEEb(addr);
   prog_eeb(addr,val);
}
