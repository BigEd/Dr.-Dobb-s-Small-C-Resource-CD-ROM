From G.J.Flanagan@uk.ac.newcastle Tue Dec 11 00:20:22 1990
Via: uk.ac.newcastle; Tue, 11 Dec 90 00:20:19 GMT
Received: from uk.ac.ncl.mts by uk.ac.newcastle; Mon, 10 Dec 90 16:02:44 GMT
Date:     Mon, 10 Dec 90  15:58:20 GMT
From: G.J.Flanagan@uk.ac.newcastle
To: syspds
Message-Id: <emu-ov07.s.1990.1210.155822.mde7@uk.ac.ncl.mts>
Status: OR

Here follows a13bp.bas
 
   10REM BASIC program to configure Custom Rom
   20REM (C) Rob Scott 1989
   30:
   40MODE 7
   50DIM rom &20D0
   60OSCLI"L.Custom1 "+STR$~rom
   70PROCvars
   80REPEAT
   90selection%=FNmenu
  100PROCconfigure(selection%)
  110UNTIL selection%=12
  120saved%=FALSE
  130PROCsave
  140IF NOT saved% THEN 80
  150END
  160:
  170DEFPROCconfigure(sel%)
  180IF sel%>12 OR sel%<1 THEN VDU7:ENDPROC
  190ON sel% GOSUB 220,230,240,250,260,270,280,290,300,310,320,330
  200ENDPROC
  210:
  220PROCbanner:RETURN
  230PROCprinter:RETURN
  240PROCignore:RETURN
  250PROCtransmit:RETURN
  260PROCreceive:RETURN
  270PROCdelay:RETURN
  280PROCrepeat:RETURN
  290PROCdata:RETURN
  300PROCloudness:RETURN
  310PROCkbdstat:RETURN
  320PROCprefix:RETURN
  330PROCdummy:RETURN
  340:
  350DEF PROCbanner
  360REPEAT
  370CLS
  380PROCprintbanner
  390PRINT'"Enter new break banner"
  400INPUTLINE'A$
  410IF LEN A$>35 VDU7:PRINT"STRING TOO LONG":GOTO390
  420PRINT'CHR$(132);CHR$(157);CHR$(134);A$
  430UNTIL FNprompt("Accept (Y/N)","NnYy")
  440$header=LEFT$(A$+STRING$(35," "),35)
  450ENDPROC
  460:
  470DEFPROCloudness
  480?loudness=FNswap(?loudness,144,216)
  490ENDPROC
  500:
  510DEFFNswap(val%,a%,b%)
  520IF val%=a% THEN =b% ELSE =a%
  530:
  540DEFPROCkbdstat
  550IF ?keyboard=&20 THEN ?keyboard=&30:ENDPROC
  560IF ?keyboard=&30 THEN ?keyboard=&A0 ELSE ?keyboard=&20
  570ENDPROC
  580:
  590DEFPROCdata
  600IF ?data<28 THEN ?data=?data+4 ELSE ?data=0
  610ENDPROC
  620:
  630DEFPROCkbd
  640IF ?keyboard=&20 THEN PRINT"Caps lock" ELSE IF ?keyboard=&30 THEN PRINT"No caps" ELSE PRINT"Shift caps"
  650ENDPROC
  660:
  670DEFPROCprefix
  680PRINTTAB(8,20)"Enter new prefix :";:A=GET:A$=CHR$(A AND &DF)
  690IF A$>"Z" OR A$<"A" THEN VDU7:ENDPROC
  700?prefix=ASC(A$)
  710DEFPROCdummy
  720ENDPROC
  730:
  740DEFPROCdelay
  750PRINTTAB(6,20)"Enter new delay (0-255):";
  760?delay=FNnumber
  770ENDPROC
  780:
  790DEFPROCrepeat
  800PRINTTAB(2,20)"Enter new repeat value (0-255):";
  810?repeat=FNnumber
  820ENDPROC
  830:
  840DEFFNnumber
  850INPUT""A$
  860IF VAL A$<0 OR VAL A$>255 THEN VDU7:PRINT'"Please re-enter :":GOTO 850
  870=VAL A$
  880:
  890DEFPROCignore
  900PRINTTAB(2,20)"Enter new ignore value (0-255):";
  910?ignore=FNnumber
  920ENDPROC
  930:
  940DEFPROCreceive
  950IF ?receive<8 THEN ?receive=?receive+1 ELSE ?receive=1
  960ENDPROC
  970:
  980DEFPROCtransmit
  990IF ?transmit<8 THEN ?transmit=?transmit+1 ELSE ?transmit=1
 1000ENDPROC
 1010:
 1020DEFPROCprinter
 1030?printer=FNswap(?printer,1,2)
 1040ENDPROC
 1050:
 1060:
 1070DEFFNprompt(a$,b$)
 1080PRINTTAB(20-LEN(a$)/2,24);a$;
 1090REPEAT
 1100a$=GET$
 1110UNTIL INSTR(b$,a$)
 1120=INSTR(b$,a$)>2
 1130:
 1140DEF FNmenu
 1150CLS
 1160PROCprintbanner
 1170RESTORE
 1180FOR option%=1 TO 12
 1190READ option$
 1200PRINTTAB(2,option%+6);CHR$(option%+64);"  ";option$
 1210NEXT
 1220PROCstatus
 1230=GET-64
 1240:
 1250DEF PROCprintbanner
 1260PRINT''''CHR$(132);CHR$(157);CHR$(134);$(rom+&7B)
 1270ENDPROC
 1280:
 1290DEF PROCstatus
 1300PRINTTAB(30,8);:IF ?printer=1 THEN PRINT"PARALLEL" ELSE PRINT "SERIAL"
 1310PRINTTAB(30,9);STR$(?ignore)
 1320PRINTTAB(30,10);FNbaud(?transmit)
 1330PRINTTAB(30,11);FNbaud(?receive)
 1340PRINTTAB(30,12);STR$(?delay)
 1350PRINTTAB(30,13);STR$(?repeat)
 1360PRINTTAB(30,14);:PROCdatatype
 1370PRINTTAB(30,15);:PROCvolume
 1380PRINTTAB(30,16);:PROCkbd
 1390PRINTTAB(30,17);CHR$(?prefix)
 1400ENDPROC
 1410:
 1420DEFPROCvolume
 1430IF ?loudness=144 THEN PRINT"Loud" ELSE PRINT"Quiet"
 1440ENDPROC
 1450:
 1460DEFFNbaud(loc)
 1470RESTORE 1840
 1480FOR I%=1 TO loc
 1490READ baud
 1500NEXT
 1510=baud
 1520:
 1530DEFPROCdatatype
 1540RESTORE 1850
 1550X%=-4
 1560REPEAT:X%=X%+4:READ A$:UNTILX%=?data
 1570PRINTA$
 1580ENDPROC
 1590:
 1600DEF PROCvars
 1610header   = rom+&7B
 1620printer  = rom+&3A7
 1630ignore   = rom+&3AE
 1640transmit = rom+&3BC
 1650receive  = rom+&3B5
 1660delay    = rom+&3C3
 1670repeat   = rom+&3CA
 1680data     = rom+&3D3
 1690loudness = rom+&3E5
 1700keyboard = rom+&3DC
 1710prefix   = rom+&266
 1720ENDPROC
 1730:
 1740DEF PROCsave
 1750*FX15
 1760CLS
 1770INPUT''"Enter filename :"fname$
 1780IF fname$="" THEN ENDPROC
 1790OSCLI"SAVE "+fname$+" "+STR$~rom+" "+STR$~(rom+&20D0)+" D9CD 8000"
 1800saved%=TRUE
 1810ENDPROC
 1820:
 1830DATA "Title Banner",Printer,"Printer Ignore Character","Transmit Baud Rate","Receive Baud Rate","Keyboard Delay","Keyboard Auto Repeat","RS423 Data Type","Bell (VDU 7) Amplitude","Keyboard Status","Command Prefix","Save Configured Rom"
 1840DATA 75,150,300,1200,2400,4800,9600,19200
 1850DATA "7D EP 2S","7D OP 2S","7D EP 1S","7D OP 1S","8D NP 2S","8D NP 1S","8D EP 1S","8D OP 1S"


