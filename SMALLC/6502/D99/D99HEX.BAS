  100 REM ** Binary to Intel Hex converter **
  120 REM ** Version 2 November 1985 **
  140   
  160 CLOSE#0
  180   
  200 DIM data 31,block 16,binfile 32
  220   
  240 INPUT "Name of input binary file? " $binfile
  260 INPUT "Name of output hex file?   " out$
  280 in% = OPENIN($binfile)
  300 IF in%=0 THEN PRINT"Input file does not exist":END
  320 out%= OPENOUT(out$)
  340   
  360 address% = FNloadaddress
  380   
  400 REPEAT
  420   PRINT"At address ";~address%
  440   PROCwriterecord
  460   UNTIL EOF#in%
  480   
  500 REM write EOF record
  520   
  540 BPUT#out%,ASC(":")
  560 FOR I% = 1 TO 3
  580   PROCputhex(0)
  600   NEXT
  620 PROCputhex(1)
  640 PROCputhex(&FF)
  660 BPUT#out%,&0D
  680   
  700 CLOSE#0
  720   
  740 END
  760   
  780 DEF PROCputhex(byte%)
  800 byte% = byte% AND &FF
  820 checksum% = (checksum% + byte%) AND &FF
  840 PROCputnibble(byte% DIV 16)
  860 PROCputnibble(byte%)
  880 ENDPROC
  900   
  920 DEF PROCputnibble(nibble%)
  940 nibble% = nibble% AND &0F
  960 IF nibble%<10 THEN BPUT#out%,nibble%+ASC("0") ELSE BPUT#out%,nibble%+ASC("A")-10
  980 ENDPROC
 1000   
 1020 DEF PROCwriterecord
 1040 LOCAL I%,J%
 1060 checksum% = 0
 1080 BPUT#out%,ASC(":")
 1100 I% = 0
 1120 REPEAT
 1140   data?I% = BGET#in%
 1160   I% = I% + 1
 1180   UNTIL I%=32 OR EOF#in%
 1200 PROCputhex(I%)
 1220 PROCputhex(address% DIV 256)
 1240 PROCputhex(address%)
 1260 PROCputhex(0)
 1280 FOR J% = 0 TO I%-1
 1300   PROCputhex(data?J%)
 1320   NEXT
 1340 recsum% = -checksum%
 1360 PROCputhex(recsum%)
 1380 BPUT#out%,&0D
 1400 address% = address% + I%
 1420 ENDPROC
 1440  
 1460 DEF FNloadaddress
 1480 block?0 = binfile AND &FF
 1500 block?1 = binfile DIV 256
 1520 A%=5:X%=block AND &FF:Y%=block DIV 256
 1540 CALL &FFDD
 1560 =block?2+block?3*256
