  100 REM Intel hex to binary converter
  120 REM Version 2 November 1985
  140   
  160 CLOSE#0
  180 DIM image 20480
  200  
  220 INPUT "Name of input hex file?    " in$
  240 INPUT "Name of output binary file? " out$
  260 in%=OPENIN(in$)
  280 IF in%=0 THEN PRINT"Input file does not exist":END
  300  
  320 imagesize% = 0
  340 record% = 0
  360 REPEAT
  380   REPEAT
  400     UNTIL (BGET#in%=ASC(":")) OR EOF#in%
  420   IF EOF#in% THEN PROCsave:CLOSE#0:END ELSE PROCreadrecord
  440   UNTIL FALSE
  460   
  480 END
  500   
  520   
  540 DEF PROCreadrecord
  560 LOCAL count%,address%,storedcsum%,type%
  580 record%=record%+1
  600 csum%=0
  620 count%=FNgetbyte
  640 address%=FNgetbyte*256+FNgetbyte
  660 IF record%=1 THEN base% = address%
  680 PRINT"Record ";record%;" : Size ";count%;", address $";~address%
  700   
  720 type% = FNgetbyte
  740 IF type%<>0 AND type%<>1 THEN PRINT"*** Unknown record type ";type%;" detected":CLOSE#0:END
  760 IF type% <> 0 THEN PRINT"+++ End-of-file record detected":ENDPROC
  780 PROCreaddata(count%)
  800 storedcsum% = FNgetbyte
  820 IF csum%<>0 THEN PRINT"*** Checksum error":CLOSE#0:END
  840 ENDPROC
  860   
  880 DEF FNgetbyte
  900 LOCAL value%
  920 value%=FNgetnibble*16 + FNgetnibble
  940 csum% = (csum% + value%) AND &FF
  960 =value%
  980   
 1000 DEF FNgetnibble
 1020 LOCAL char%
 1040 char% = BGET#in%
 1060 IF char%>=ASC("A") THEN =char%-ASC("A")+10 ELSE =char%-ASC("0")
 1080   
 1100 DEF PROCreaddata(count%)
 1120 LOCAL I%
 1140 pointer% = address% - base%
 1160 FOR I% = 1 TO count%
 1180   image?pointer% = FNgetbyte
 1200   address% = address% + 1
 1220   pointer% = pointer% + 1
 1240   NEXT
 1260 IF pointer% > imagesize% THEN imagesize% = pointer%
 1280 ENDPROC
 1300   
 1320 DEF PROCsave
 1340 loadaddress% = base% OR &FFFF0000
 1360 OSCLI("*SAVE "+out$+" "+STR$~image+" "+STR$~(imagesize%+image)+" "+STR$~loadaddress%+" "+STR$~loadaddress%)
 1380 ENDPROC
