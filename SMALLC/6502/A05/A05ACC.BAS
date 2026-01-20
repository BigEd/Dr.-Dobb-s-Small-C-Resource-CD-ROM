   10*| REM Program: Account (AR307)
   20*| REM Author:  Mike Brudenell, (C)
   30*| REM Machine: BBC Micro with disk drive and Basic 2
   40*|
   50ON ERROR GOTO 270
   60*DISK
   70*TV0,1
   80*FX4,1
   90*FX229,1
  100*| REM *** In next line, change "TRUE" to "FALSE" if running on a BBC model B without 6502 Second Processor on or Shadow RAM.
  110BigModel% = TRUE
  120*| REM
  130IF BigModel% THEN MODE 128+0:entries%=27:bottom%=31 ELSE MODE 128+3:entries%=20:bottom%=24
  140A%=132:IF BigModel% AND ((USR(&FFF4) AND &FFFF00) < &7C0000) THEN VDU 7:PRINT"Insufficient memory for Big Model."''"Please see comment in first few lines of program.":GOTO 200
  150VDU 23,0,10,7,0;0;0;
  160DIM date% entries%,check% entries%,account$(entries%),number%(entries%),credit(entries%),debit(entries%),balance(entries%),total(2),start% 6,width% 6
  170PROCinit
  180PROCcommand
  190MODE 3
  200@%=&90A
  210*FX 4,0
  220*FX 229,0
  230END
  240
  250
  260
  270IF ERR=195 THEN CLOSE#0:PRINTTAB(0,bottom%)"Output error: Aborted";:VDU 7:handle%=GET:PRINTTAB(0,bottom%)SPC28;:GOTO 180
  280IF ERR=190 THEN CLOSE#0:PRINTTAB(0,bottom%)"Catalog full: Aborted";:VDU 7:handle%=GET:PRINTTAB(0,bottom%)SPC28;:GOTO 180
  290IF ERR=223 THEN CLOSE#0:PRINTTAB(0,bottom%)"Wrong size datafile: Aborted";:VDU 7:handle%=GET:PRINTTAB(0,bottom%)SPC28;:RUN
  300*FX 4,0
  310*FX 229,0
  320@%=&90A:REPORT:PRINT" at line ";ERL
  330CLOSE#0:VDU20
  340END
  350
  360
  370
  380DEF FNgpi(len%,valid$)
  390LOCAL a$,b%
  400a$=STRING$(20," ") :a$=""
  410PRINTSTRING$(len%,".")STRING$(len%,CHR$8);
  420b%=GET
  430IF b%<>13 AND b%<>27 THEN GOTO 470
  440PRINTSTRING$(LEN(a$),CHR$8)STRING$(len%," ")STRING$(len%,CHR$8);
  450IF b%=27 THEN a$=CHR$27
  460=a$
  470IF b%=127 AND a$="" THEN GOTO 420
  480IF b%=21 THEN PRINTSTRING$(LENa$,CHR$8)STRING$(len%,".")STRING$(len%,CHR$8);:a$="":GOTO 420
  490IF b%=127 THEN a$=LEFT$(a$,LENa$-1):PRINTCHR$b%"."CHR$8;:GOTO 420
  500IF b%>=ASC"A" AND b%<=ASC"Z" THEN b%=b%+32
  510IF (a$="" OR INSTR(" (.&-/",RIGHT$(a$,1))) AND (b%>=ASC"a" AND b%<=ASC"z") THEN b%=b%-32
  520IF LENa$=len% OR INSTR(valid$,CHR$b%)=0 THEN VDU 7:GOTO 420
  530a$=a$+CHR$b%:IF b%>=32 THEN PRINTCHR$b%; ELSE IF b%<>21 THEN PRINT" ";
  540GOTO 420
  550
  560
  570
  580DEF PROCscreen
  590LOCAL row%,field%
  600CLS
  610@%=&20208:PRINTTAB(0,0)"Month: "account$(0)TAB(41,0)"Initial balance: "balance(0);
  620PRINTTAB(0,2)"Date  Chk  Account               No.    Credit     Debit   Balance";
  630FOR row%=1 TO entries%
  640IF date%?row%=0 THEN 680
  650FOR field%=0 TO 6
  660PROCprint_field(field%,row%)
  670NEXT field%
  680NEXT row%
  690PRINTTAB(0,bottom%-1)STRING$(66,"_");
  700PRINTTAB(30,bottom%)"Totals: "total(0)"  "total(1)"  "total(2);
  710ENDPROC
  720
  730
  740
  750DEF PROCinit
  760LOCAL row%
  770filename$="":altered%=FALSE
  780FOR row%=0 TO entries%
  790date%?row%=0
  800check%?row%=FALSE
  810account$(row%)=STRING$(20," "):account$(row%)=""
  820number%(row%)=0
  830credit(row%)=0
  840debit(row%)=0
  850balance(row%)=0
  860NEXT
  870date%?0=&FF
  880FOR row%=0 TO 2
  890total(row%)=0
  900NEXT
  910capitals$="ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  920small$="abcdefghijklmnopqrstuvwxyz"
  930digits$="0123456789"
  940RESTORE
  950FOR row%=0 TO 6
  960READ start%?row%,width%?row%
  970NEXT
  980DATA 1,2, 7,1, 11,20, 33,3, 38,8, 48,8, 58,8
  990PROCrecalculate
 1000ENDPROC
 1010
 1020
 1030
 1040DEF PROCrecalculate
 1050LOCAL row%
 1060FOR row%=0 TO 2
 1070total(row%)=0
 1080NEXT
 1090FOR row%=1 TO entries%
 1100total(0)=total(0)+credit(row%)
 1110total(1)=total(1)+debit(row%)
 1120balance(row%)=balance(row%-1)+credit(row%)-debit(row%)
 1130NEXT
 1140total(2)=balance(0)+total(0)-total(1)
 1150PROCscreen
 1160ENDPROC
 1170
 1180
 1190
 1200DEF PROCdelete(row%)
 1210LOCAL r%
 1220IF row%=entries% THEN GOTO 1310
 1230FOR r%=row% TO entries%-1
 1240date%?r%=date%?(r%+1)
 1250check%?r%=check%?(r%+1)
 1260account$(r%)=account$(r%+1)
 1270number%(r%)=number%(r%+1)
 1280credit(r%)=credit(r%+1)
 1290debit(r%)=debit(r%+1)
 1300NEXT
 1310date%?entries%=0
 1320check%?entries%=FALSE
 1330account$(entries%)=""
 1340number%(entries%)=0
 1350credit(entries%)=0
 1360debit(entries%)=0
 1370PROCrecalculate
 1380ENDPROC
 1390
 1400
 1410
 1420DEF PROCcommand
 1430LOCAL key$
 1440REPEAT
 1450REPEAT UNTIL NOT INKEY(-113)
 1460PRINTTAB(0,bottom%)"Command: ";:key$=FNgpi(1,"QCLSEP?HT")
 1470PRINTTAB(0,bottom%)SPC28;
 1480IF key$="L" THEN PROCload
 1490IF key$="S" THEN PROCsave
 1500IF key$="T" THEN PROCterminate
 1510IF key$="E" THEN PROCedit
 1520IF key$="P" THEN PROCprint
 1530IF key$="C" THEN IF FNconfirmed("CLEAR PAGE") THEN PROCinit
 1540IF key$="?" THEN PROCcat
 1550IF key$="H" THEN PRINTTAB(0,bottom%)"Commands: Q LST? EC P";:key$=GET$:key$="":PRINTTAB(0,bottom%)SPC28;
 1560UNTIL key$="Q"
 1570IF altered% THEN IF NOT FNconfirmed("LOSE THIS") THEN 1440
 1580ENDPROC
 1590
 1600
 1610
 1620DEF PROCsave
 1630LOCAL buffer$,handle%,row%
 1640REPEAT
 1650PRINTTAB(0,bottom%)"Save ";:IF filename$="" THEN PRINT"in file: "; ELSE PRINT"[";filename$;"]: ";
 1660buffer$=FNgpi(7,capitals$+small$+digits$)
 1670PRINTTAB(0,bottom%)SPC28;
 1680UNTIL filename$<>"" OR buffer$<>""
 1690IF ASCbuffer$=27 THEN PRINTTAB(0,bottom%)SPC28;:ENDPROC
 1700IF buffer$<>"" THEN filename$=buffer$
 1710handle%=OPENUP("`."+filename$)
 1720IF handle%<>0 THEN CLOSE#handle% :OSCLI"*DELETE `."+filename$
 1730handle%=OPENOUT("`."+filename$)
 1740IF handle%=0 THEN PRINTTAB(0,bottom%)"Output error: Aborted";:VDU 7:handle%=GET:PRINTTAB(0,bottom%)SPC28;:ENDPROC
 1750FOR row%=0 TO entries%
 1760PRINT#handle%,date%?row%,check%?row%,account$(row%),number%(row%),credit(row%),debit(row%)
 1770NEXT
 1780PRINT#handle%,balance(0)
 1790CLOSE#handle%
 1800altered%=FALSE:ENDPROC
 1810
 1820
 1830
 1840DEF PROCload
 1850LOCAL handle%,row%
 1860IF altered% THEN IF NOT FNconfirmed("LOSE THIS") THEN ENDPROC
 1870REPEAT
 1880PRINTTAB(0,bottom%)"Load from file: ";:filename$=FNgpi(7,capitals$+small$+digits$)
 1890PRINTTAB(0,bottom%)SPC28;
 1900UNTIL filename$<>""
 1910IF ASCfilename$=27 THEN PRINTTAB(0,bottom%)SPC28;:ENDPROC
 1920handle%=OPENIN("`."+filename$)
 1930IF handle%=0 THEN PRINTTAB(0,bottom%)"Input error: Aborted";:VDU 7:handle%=GET:PRINTTAB(0,bottom%)SPC28;:ENDPROC
 1940LOCAL row%
 1950FOR row%=0 TO entries%
 1960INPUT#handle%,date%?row%,check%?row%,account$(row%),number%(row%),credit(row%),debit(row%)
 1970NEXT
 1980INPUT#handle%,balance(0)
 1990IF NOT EOF#handle% THEN CLOSE#0:PRINTTAB(0,bottom%)"Wrong size datafile: Aborted";:VDU 7:handle%=GET:PRINTTAB(0,bottom%)SPC28;:RUN
 2000CLOSE#handle%
 2010PROCrecalculate
 2020altered%=FALSE:ENDPROC
 2030
 2040
 2050
 2060DEF PROCterminate
 2070LOCAL filename$,handle%
 2080REPEAT
 2090PRINTTAB(0,bottom%)"Terminate file: ";:filename$=FNgpi(7,capitals$+small$+digits$)
 2100PRINTTAB(0,bottom%)SPC28;
 2110UNTIL filename$<>""
 2120IF ASCfilename$=27 THEN PRINTTAB(0,bottom%)SPC28;:ENDPROC
 2130IF NOT FNconfirmed("Terminate") THEN ENDPROC
 2140handle%=OPENIN("`."+filename$)
 2150IF handle%<>0 THEN CLOSE#handle%:OSCLI"*ACCESS `."+filename$+" L" ELSE PRINTTAB(0,bottom%)"Terminate error: Aborted";:VDU 7:handle%=GET:PRINTTAB(0,bottom%)SPC28;
 2160ENDPROC
 2170
 2180
 2190
 2200DEF PROCprint
 2210LOCAL code%,space%,time%,row%,col%,A%:A%=135
 2220PRINTTAB(0,bottom%)SPC28TAB(0,bottom%)"Testing printer";:space%=ADVAL(-4):VDU 2,1,0,1,0,1,0,3:time%=TIME+500:REPEATUNTILTIME>time%ORADVAL(-4)>=space%
 2230IFADVAL(-4)<space%THENPRINTTAB(0,bottom%)SPC28TAB(0,bottom%)"Printer off-line: Aborted";:VDU 7:code%=GET:PRINTTAB(0,bottom%)SPC28;:ENDPROC
 2240PRINTTAB(0,bottom%)SPC28TAB(0,bottom%);
 2250VDU 2,1,13
 2260FOR row%=0 TO bottom%
 2270FOR col%=0 TO 65
 2280VDU31,col%,row%,1,((USR(&FFF4) AND &FFFF) DIV &100)
 2290NEXT
 2300VDU1,13
 2310NEXT
 2320VDU1,12,3
 2330ENDPROC
 2340
 2350
 2360
 2370DEF PROCposn(f%,r%)
 2380IF r%=0 AND f%=4 THEN PRINTTAB(7,0);:ENDPROC
 2390IF r%=0 AND f%=5 THEN PRINTTAB(58,0);:ENDPROC
 2400PRINTTAB(start%?f%,r%+2);
 2410ENDPROC
 2420
 2430
 2440
 2450DEF PROCedit
 2460LOCAL field%,row%,key%
 2470field%=0
 2480row%=1
 2490REPEAT
 2500REPEAT
 2510PROCposn(field%,row%)
 2520key%=GET
 2530IF key%=27 THEN GOTO 2590
 2540IF key%=136 THEN IF field%<>4 OR row%<>0 THEN field%=field%-1:IF field%<0 THEN field%=5:row%=row%-1
 2550IF key%=137 THEN IF (field%<>5 OR row%<>entries%) AND date%?row%<>0 THEN field%=field%+1:IF field%>5 THEN field%=0:row%=row%+1
 2560IF key%=139 AND row%>1 THEN row%=row%-1
 2570IF (key%=13 OR key%=138) AND row%<entries% AND date%?row%<>0 THEN row%=row%+1:IF date%?row%=0 OR key%=13 THEN field%=0
 2580IF (key%<>13 AND key%<135) OR key%>139 THEN VDU 7
 2590UNTIL key%=27 OR key%=135
 2600IF key%=135:IF FNupdated(field%,row%) THEN IF field%<>5 OR row%<>entries% THEN field%=field%+1:IF field%>5 THEN field%=0:row%=row%+1
 2610UNTIL key%=27
 2620PROCrecalculate
 2630ENDPROC
 2640
 2650
 2660
 2670DEF FNupdated(f%,r%)
 2680LOCAL buffer$,valid$
 2690buffer$=STRING$(20," ")
 2700valid$=STRING$(80," ")
 2710PROCposn(f%,r%)
 2720IF f%=1 THEN valid$="X " ELSE IF f%=2 OR (r%=0 AND f%=4) THEN valid$=capitals$+small$+digits$+" " ELSE valid$=digits$
 2730IF f%=4 OR f%=5 THEN valid$=valid$+"." ELSE IF f%=2 THEN valid$=valid$+"()&.,-/"
 2740IF r%=0 AND f%=4 THEN buffer$=FNgpi(width%?2,valid$) ELSE buffer$=FNgpi(width%?f%,valid$)
 2750IF ASCbuffer$=27 OR buffer$="" THEN PROCprint_field(f%,r%):=FALSE
 2760IF r%=0 AND f%=4 THEN account$(0)=buffer$:GOTO 2790 ELSE IF r%=0 THEN balance(0)=VALbuffer$:GOTO 2790
 2770IF f%=0 THEN date%?r%=VALbuffer$ ELSE IF f%=1 THEN check%?r%=(buffer$="X") ELSE IF f%=2 THEN account$(r%)=buffer$ ELSE IF f%=3 THEN number%(r%)=VALbuffer$ ELSE IF f%=4 THEN credit(r%)=VALbuffer$ ELSE debit(r%)=VALbuffer$
 2780IF f%=0 AND date%?r%=0 THEN PROCdelete(r%):altered%=TRUE:=FALSE
 2790PROCprint_field(f%,r%)
 2800altered%=TRUE:=TRUE
 2810
 2820
 2830
 2840DEF PROCprint_field(f%,r%)
 2850PROCposn(f%,r%)
 2860IF f%=0 THEN @%=&2 ELSE IF f%=3 THEN @%=&3 ELSE IF f%=4 OR f%=5 OR f%=6 THEN @%=&20208 ELSE @%=&0
 2870IF r%=0 AND f%=4 THEN PRINT account$(0); ELSE IF r%=0 AND f%=5 THEN PRINT balance(0); ELSE IF f%=4 AND credit(r%)<>0 THEN PRINT credit(r%); ELSE IF f%=5 AND debit(r%)<>0 THEN PRINT debit(r%);
 2880IF f%=0 AND date%?r%<>0 THEN PRINT date%?r%; ELSE IF f%=0 THEN PRINT"  ";
 2890IF f%=1 AND check%?r% THEN PRINT"X"; ELSE IF f%=1 THEN PRINT" ";
 2900IF f%=2 THEN PRINT account$(r%);
 2910IF f%=3 AND number%(r%)=0 THEN PRINT"   "; ELSE IF f%=3 THEN PRINT STRING$(3-LENSTR$number%(r%),"0");number%(r%);
 2920IF f%=6 THEN PRINT balance(r%);
 2930IF f%=2 OR (r%=0 AND f%=4) THEN PRINTSTRING$(20-LEN(account$(r%))," ");
 2940ENDPROC
 2950
 2960
 2970
 2980DEF FNconfirmed(mesg$)
 2990LOCAL reply$
 3000IF LENmesg$>10 THEN mesg$=LEFT$(mesg$,10)
 3010PRINTTAB(0,bottom%)SPC28TAB(0,bottom%)mesg$"? Confirm: ";
 3020reply$=FNgpi(1,"YN")
 3030PRINTTAB(0,bottom%)SPC28;
 3040=(reply$="Y")
 3050
 3060
 3070
 3080DEF PROCcat
 3090LOCAL key%
 3100CLS
 3110*CAT
 3120PRINTTAB(0,bottom%)"Press a key: ";:key%=GET
 3130PROCscreen
 3140ENDPROC
