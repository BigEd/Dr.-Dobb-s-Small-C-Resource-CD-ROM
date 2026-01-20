   10REM Function key strip printer (for Epson / compatible printers)
   20REM Model (AY266)
   30REM Mike Brudenell
   40:
   50REM Uses following printer codes:
   60REM      Esc "@" .............. Reset printer
   70REM      Esc "U" .............. Unidirectional printing on/off
   80REM      Esc "3" .............. Set line spacing to n/216"
   90REM      Esc "!" .............. Set print font
  100REM      Esc "L" .............. Dual-density bit image graphics
  110REM      Esc "S" .............. Superscript on/off
  120:
  130ON ERROR VDU 1,27,1,ASC"@", 3 : REPORT : PRINT " at line ";ERL : CLOSE#0 : END
  140:
  150DIM Text$(5,9)
  160:
  170MODE 3
  180PRINT "Function key strip printer"
  190PRINT "  (for Epson / compatible printers)"''
  200IF NOT FNprinteron THEN PRINT '"Error:  Printer not online!" : END
  210PROCreaddata
  220:
  230PRINT "Printing...  Please wait"
  240:
  250VDU 2                 : REM Enable printer
  260VDU 1,27,1,ASC"@"     : REM Reset printer
  270VDU 1,27,1,ASC"U",1,1 : REM Start unidirectional printing
  280:
  290PROCspacing(27) : VDU 1,13 : PROCcutline
  300PROCtitle(Title$)
  310PROClabels
  320PROCspacing(6) : VDU 1,13 : PROCcutline
  330:
  340VDU 1,27,1,ASC"@"     : REM Reset printer
  350VDU 1,13, 1,13, 1,13  : REM Throw a few lines
  360VDU 3                 : REM Disable printer
  370:
  380PRINT SPC(13);"Complete"'
  390END
  400:
  410:
  420:
  430DEF FNprinteron
  440LOCAL Bufffree%,Time%,Online%
  450Bufffree%=ADVAL(-4)
  460VDU 2, 1,0, 1,0, 1,0, 3
  470Time%=TIME+200
  480REPEAT
  490Online%=(ADVAL(-4) >= Bufffree%)
  500UNTIL TIME >= Time% OR Online%
  510=Online%
  520:
  530:
  540:
  550DEF PROCspacing(N%)
  560VDU 1,27,1,ASC"3",1,N% : REM Set line spacing to N%/216"
  570ENDPROC
  580:
  590:
  600:
  610DEF PROCfont(F%)
  620VDU 1,27,1,ASC"!",1,F% : REM Select font number F%
  630ENDPROC
  640:
  650:
  660:
  670DEF PROCcutline
  680LOCAL I%
  690VDU 1,27,1,ASC"L",1,156,1,3
  700FOR I%=1 TO 924 : VDU 1,&28 : NEXT
  710VDU 1,13
  720ENDPROC
  730:
  740:
  750:
  760DEF PROCprint(Label$)
  770LOCAL I%
  780IF LEN(Label$) > 1 THEN FOR I%=1 TO LEN(Label$) : VDU 1,ASC(MID$(Label$,I%,1)) : NEXT
  790ENDPROC
  800:
  810:
  820:
  830DEF PROCtitle(Title$)
  840PROCfont(49) : REM Enlarged/emphasised/elite
  850PROCprint("    "+Title$) : VDU 1,13
  860ENDPROC
  870:
  880:
  890:
  900DEF FNmaxlen(Key%)
  910LOCAL Maxlen%
  920IF Key% MOD 2 = 0 THEN Maxlen%=11 ELSE Maxlen%=12
  930=Maxlen%
  940:
  950:
  960:
  970DEF FNopenfile
  980LOCAL File$,Handle%
  990REPEAT
 1000INPUT "Enter name of data file: " File$
 1010Handle%=OPENIN(File$)
 1020IF Handle%=0 THEN PRINT "  File not found"'
 1030UNTIL Handle% <> 0
 1040=Handle%
 1050:
 1060:
 1070:
 1080DEF FNreadline(Handle%)
 1090LOCAL Line$, Ch%
 1100Line$=STRING$(80, " ") : Line$=""
 1110REPEAT
 1120IF EOF#Handle% THEN PRINT '"Error:  Unexpected EOF in data file!"' : END
 1130Ch%=BGET#Handle%
 1140IF Ch% <> 13 THEN Line$=Line$ + CHR$(Ch%)
 1150UNTIL Ch%=13
 1160=Line$
 1170:
 1180:
 1190:
 1200DEF FNextract
 1210LOCAL Barpos%,Key$
 1220Key$=STRING$(12, " ")
 1230Barpos%=INSTR(Labels$, "|")
 1240IF Barpos% = 0 THEN Key$=Labels$ : Labels$="" ELSE Key$=LEFT$(Labels$, Barpos%-1) : Labels$=MID$(Labels$, Barpos%+1)
 1250=Key$
 1260:
 1270:
 1280:
 1290DEF PROCdivide(Row%, Key%, Label$)
 1300LOCAL Commapos%,Maxlen%
 1310Commapos%=INSTR(Label$, ",")
 1320IF Commapos%=0 THEN Text$(Row%,Key%)=Label$ : Text$(Row%+1,Key%)="" ELSE Text$(Row%,Key%)=LEFT$(Label$, Commapos%-1) : Text$(Row%+1,Key%)=MID$(Label$, Commapos%+1)
 1330Maxlen%=FNmaxlen(Key%)
 1340IF LEN(Text$(Row%,Key%)) > Maxlen% THEN Text$(Row%,Key%)=LEFT$(Text$(Row%,Key%), Maxlen%) : PRINT "Warning:  Label part ";Row%;" too long for key ";Key%;"!"
 1350IF LEN(Text$(Row%+1,Key%)) > Maxlen% THEN Text$(Row%+1,Key%)=LEFT$(Text$(Row%+1,Key%), Maxlen%) : PRINT "Warning:  Label part ";Row%+1;" too long for key ";Key%;"!"
 1360ENDPROC
 1370:
 1380:
 1390:
 1400DEF PROCsplit(Key%, Labels$)
 1410PROCdivide(4, Key%, FNextract)
 1420PROCdivide(2, Key%, FNextract)
 1430PROCdivide(0, Key%, FNextract)
 1440IF FNextract <> "" THEN PRINT "Warning:  Unexpected ""|"" found for key ";Key%;"!"
 1450ENDPROC
 1460:
 1470:
 1480:
 1490DEF PROCreaddata
 1500LOCAL Handle%,Key%
 1510Handle%=FNopenfile
 1520Title$=FNreadline(Handle%)
 1530FOR Key%=0 TO 9
 1540PROCsplit(Key%, FNreadline(Handle%))
 1550NEXT
 1560IF NOT EOF#Handle% THEN PRINT '"Warning:  EOF expected but not found in data file!"'
 1570CLOSE#Handle%
 1580ENDPROC
 1590:
 1600:
 1610:
 1620DEF PROCspc
 1630LOCAL I%
 1640VDU 1,27,1,ASC"L",1,42,1,0
 1650FOR I%=1 TO 42 : VDU 1,0 : NEXT
 1660ENDPROC
 1670:
 1680:
 1690:
 1700DEF PROCtoleft
 1710PROCspacing(0) : VDU 1,13 : PROCspacing(18)
 1720ENDPROC
 1730:
 1740:
 1750:
 1760DEF PROCtop
 1770LOCAL Key%,I%
 1780PROCspc
 1790VDU 1,27,1,ASC"L",1,114,1,3
 1800VDU 1,0, 1,0, 1,&3C, 1,&20, 1,&20, 1,&20, 1,&20
 1810FOR Key%=0 TO 9 STEP 2
 1820FOR I%=1 TO 77 : VDU 1,&20 : NEXT
 1830VDU 1,&20, 1,&20, 1,&3C, 1,&20, 1,&20, 1,&20, 1,&20
 1840FOR I%=1 TO 84 : VDU 1,&20 : NEXT
 1850IF Key% <> 8 THEN VDU 1,&20, 1,&20, 1,&3C, 1,&20, 1,&20, 1,&20, 1,&20
 1860NEXT
 1870VDU 1,&20, 1,&20, 1,&3C, 1,0, 1,0, 1,0, 1,0
 1880PROCtoleft
 1890ENDPROC
 1900:
 1910:
 1920:
 1930DEF PROCmid
 1940LOCAL Key%,I%
 1950PROCspc
 1960VDU 1,27,1,ASC"L",1,114,1,3
 1970FOR Key%=0 TO 9 STEP 2
 1980VDU 1,0, 1,0, 1,&FC, 1,0, 1,0, 1,0, 1,0
 1990FOR I%=1 TO 77 : VDU 1,0 : NEXT
 2000VDU 1,0, 1,0, 1,&FC, 1,0, 1,0, 1,0, 1,0
 2010FOR I%=1 TO 84 : VDU 1,0 : NEXT
 2020NEXT
 2030VDU 1,0, 1,0, 1,&FC, 1,0, 1,0, 1,0, 1,0
 2040PROCtoleft
 2050ENDPROC
 2060:
 2070:
 2080:
 2090DEF PROCline
 2100LOCAL Key%,I%
 2110PROCspc
 2120VDU 1,27,1,ASC"L",1,114,1,3
 2130VDU 1,0, 1,0, 1,&FC, 1,&20, 1,&20, 1,&20, 1,&20
 2140FOR Key%=0 TO 9 STEP 2
 2150FOR I%=1 TO 77 : VDU 1,&20 : NEXT
 2160VDU 1,&20, 1,&20, 1,&FC, 1,&20, 1,&20, 1,&20, 1,&20
 2170FOR I%=1 TO 84 : VDU 1,&20 : NEXT
 2180IF Key% <> 8 THEN VDU 1,&20, 1,&20, 1,&FC, 1,&20, 1,&20, 1,&20, 1,&20
 2190NEXT
 2200VDU 1,&20, 1,&20, 1,&FC, 1,0, 1,0, 1,0, 1,0
 2210PROCtoleft
 2220ENDPROC
 2230:
 2240:
 2250:
 2260DEF PROCbottom
 2270LOCAL Key%,I%
 2280PROCspc
 2290VDU 1,27,1,ASC"L",1,114,1,3
 2300VDU 1,0, 1,0, 1,&F8, 1,&08, 1,&08, 1,&08, 1,&08
 2310FOR Key%=0 TO 9 STEP 2
 2320FOR I%=1 TO 77 : VDU 1,&08 : NEXT
 2330VDU 1,&08, 1,&08, 1,&F8, 1,&08, 1,&08, 1,&08, 1,&08
 2340FOR I%=1 TO 84 : VDU 1,&08 : NEXT
 2350IF Key% <> 8 THEN VDU 1,&08, 1,&08, 1,&F8, 1,&08, 1,&08, 1,&08, 1,&08
 2360NEXT
 2370VDU 1,&08, 1,&08, 1,&F8, 1,0, 1,0, 1,0, 1,0
 2380PROCtoleft
 2390ENDPROC
 2400:
 2410:
 2420:
 2430DEF PROCprintrow(Row%)
 2440LOCAL Key%,Maxlen%,Text$
 2450IF Row% = 0 THEN PROCprint(" Ctrl  ") ELSE IF Row% = 2 THEN PROCprint("Shift  ") ELSE PROCprint("       ")
 2460FOR Key%=0 TO 9
 2470Maxlen%=FNmaxlen(Key%)
 2480Text$=Text$(Row%,Key%)
 2490Text$=STRING$((Maxlen%-LEN(Text$)) DIV 2, " ") + Text$ + STRING$((Maxlen%-LEN(Text$)) DIV 2, " ")
 2500IF LEN(Text$) <> Maxlen% THEN VDU 1,27,1,ASC"L",1,3,1,0, 1,0, 1,0, 1,0
 2510PROCprint(Text$+" ")
 2520IF LEN(Text$) <> Maxlen% THEN VDU 1,27,1,ASC"L",1,4,1,0, 1,0, 1,0, 1,0, 1,0
 2530NEXT
 2540ENDPROC
 2550:
 2560:
 2570:
 2580DEF PROClabels
 2590PROCspacing(18)
 2600PROCfont(20) : REM Condensed/double-strike
 2610VDU 1,27,1,ASC"S",1,0 : REM superscript on
 2620PROCtop : VDU 1,13
 2630PROCmid : PROCprintrow(0) : VDU 1,13
 2640PROCmid : PROCprintrow(1) : VDU 1,13
 2650PROCline : VDU 1,13
 2660PROCmid : PROCprintrow(2) : VDU 1,13
 2670PROCmid : PROCprintrow(3) : VDU 1,13
 2680PROCline : VDU 1,13
 2690PROCmid : PROCprintrow(4) : VDU 1,13
 2700PROCmid : PROCprintrow(5) : VDU 1,13
 2710PROCbottom : VDU 1,13
 2720ENDPROC
