   10REM Handirom loading program
   20REM by Rob Scott & Andy Winston
   30:
   40MODE7
   50IF PAGE>&1800 THEN PAGE=&1800:CHAIN"HI"
   60?&FE62=&FF
   70test%=FALSE
   80ONERRORGOTO100
   90*HOFF
  100ONERROROFF
  110*DIR $
  120maxfiles%=9:filelen%=8
  130rombase%=&2A1
  140DIM osbuff% 5,rombuf% &4000,osblock &20,buffer% 64,title$(maxfiles%),titles% 520,onoff$(1),yesno$(1),code 100,ram%(16)
  150onoff$(0)="ON ":onoff$(1)="OFF"
  160yesno$(0)="YES":yesno$(1)="NO "
  170VDU23,1,0;0;0;0;
  180PROCtitle
  190PROCchkram
  200PROCidentifymc
  210PROCintro
  220PROCspeed
  230PROCsound
  240PROCoption
  250PROCwords
  260:
  270GOTO 3200
  280:
  290DEFPROCintro
  300CLS
  310PRINT''"The various features of the ROM version of the Newcastle Handisystem can be     called at any time using commands which start with a *."
  320PRINT'" e.g. *BEEP   will turn on the bleeps."
  330PRINT"      *SPEED 0 will slow the scan rate               to its minimum value."
  340PRINT'"A list of the commands available can be seen by typing *HELP HANDIROM  or in a  shorter form,  *H.H."
  350PROCprompt
  360ENDPROC
  370:
  380DEFPROCchkram
  390FOR I%=0 TO 15
  400?&FE60=I%
  410IF FNramtest(I%) THEN ram%(I%)=TRUE:test%=TRUE ELSE ram%(I%)=FALSE
  420NEXT
  430IF test% ENDPROC
  440VDU7:PRINT''''CHR$141;"        NO SIDEWAYS RAM FOUND"'CHR$141;"        NO SIDEWAYS RAM FOUND":END
  450ENDPROC
  460:
  470DEFPROCspeed
  480CLS
  490PROCcen("Please select the initial speed now",3):PROCcen("0 is very slow, 9 quite fast!",4)
  500I=0:REPEAT
  510PROCcen("SPEED"+CHR$131+STR$(I),13):A%=FNpause:I=I+1
  520UNTIL I>9 OR A%
  530IF I>9AND NOT A% THEN 500
  540I=I-1:OSCLI"HSPEED "+STR$(I)
  550PROCprompt
  560ENDPROC
  570:
  580DEFPROCsound
  590CLS
  600PROCcen("Would you like the sound on or off?",3)
  610I=0:REPEAT
  620PROCcen("SOUND"+CHR$131+onoff$(I),13):A%=FNpause:I=I+1
  630UNTIL I>1 OR A%
  640IF I=2AND NOT A%THEN610
  650IF I=1 THEN OSCLI"BEEP" ELSE OSCLI"NOBEEP"
  660PROCprompt
  670ENDPROC
  680:
  690DEFPROCoption
  700CLS
  710PROCcen("Now select the wordprocessor option",3)
  720I=0:REPEAT
  730PROCcen("WORDPROCESSOR"+CHR$131+onoff$(I),13):A%=FNpause:I=I+1
  740UNTIL A% OR I>1
  750IF I=2ANDNOTA% THEN 720
  760IF I=1 THEN OSCLI"WPON" ELSE OSCLI"WPOFF"
  770PROCprompt
  780ENDPROC
  790:
  800DEFPROCwords
  810*DIR W
  820CLS
  830PROCcen("Finally select the wordlist",3)
  840PROCcen("you would like to use",5)
  850num_files%=FNget_files
  860PROCquicksort(1,num_files%)
  870FOR file%=1 TO num_files%
  880xpos%=12*((file%-1)MOD 3)
  890ypos%=15+((file%-1)DIV3)
  900PRINTTAB(xpos%,ypos%-7);" ";file%;CHR$134
  910PRINTTAB(xpos%+3,ypos%-7);title$(file%);CHR$135
  920NEXT
  930IF files%=1 I=1:GOTO1000
  940I=1:REPEAT
  950PROCcen(" WORDLIST"+CHR$131+STR$(I),13):A%=FNpause:I=I+1
  960UNTIL I>files% OR A%
  970IF I>files% AND NOT A% THEN940
  980I=I-1
  990PROCcen("Loading "+title$(I)+" _ Please wait",18)
 1000OSCLI"WLOAD "+title$(I)
 1010ENDPROC
 1020:
 1030DEFFNget_files
 1040?osblock=0
 1050osblock!1=titles%
 1060osblock!5=maxfiles%
 1070osblock!9=0
 1080A%=8
 1090X%=osblock MOD 256:Y%=osblock DIV 256
 1100IF (USR(&FFD1) DIV &1000000)AND 1 THEN files%=maxfiles%-osblock!5 ELSE files%=maxfiles%
 1110FORI=1 TO files%
 1120?(titles%+filelen%*I)=13
 1130title$(I)=$(titles%+1+filelen%*(I-1))
 1140NEXT
 1150=files%
 1160:
 1170DEFPROCquicksort(start%,finish%)
 1180LOCALsave$,save2$,temps%,tempf%
 1190temps%=start%:tempf%=finish%
 1200save$=title$((temps%+tempf%)DIV2)
 1210REPEAT
 1220IFtitle$(temps%)<save$THENtemps%=temps%+1:GOTO1220
 1230IFsave$<title$(tempf%)THENtempf%=tempf%-1:GOTO1230
 1240IFtemps%<=tempf%THENsave2$=title$(temps%):title$(temps%)=title$(tempf%):title$(tempf%)=save2$:temps%=temps%+1:tempf%=tempf%-1
 1250UNTILtemps%>tempf%
 1260IFstart%<tempf%THENPROCquicksort(start%,tempf%)
 1270IFtemps%<finish%THENPROCquicksort(temps%,finish%)
 1280ENDPROC
 1290:
 1300DEFPROCtitle
 1310CLS:FORT%=1TO2:PRINTTAB(9,T%);CHR$141;CHR$134;"Newcastle Handirom":NEXT:PRINTTAB(0,4);CHR$129;:FORT%=1TO38:PRINTTAB(T%,4);CHR$255;:NEXT:VDU28,0,24,39,5:PRINT
 1320ENDPROC
 1330:
 1340DEFFNpause
 1350*FX15
 1360IF ADVAL (0) AND 3<>0 THEN 1360
 1370FOR wait=0 TO 20:NEXT
 1380TIME=0
 1390REPEAT
 1400A%=(ADVAL(0)AND 3)+INKEY(0)
 1410UNTIL A%<>-1 OR TIME>150
 1420IF A%<>-1 =-1ELSE =0
 1430:
 1440DEFPROCcen(A$,Y%)
 1450PRINTTAB((40-LENA$)/2,Y%);A$
 1460ENDPROC
 1470:
 1480DEFPROCprompt
 1490PROCcen(CHR$134+"Press button or any key to continue ",18)
 1500REPEAT UNTILFNpause
 1510ENDPROC
 1520:
 1530DEFPROCidentifymc
 1540LOCAL mc%
 1550mc%=FNtest
 1560IF mc%=1 PROCselectbank:PROCbbc(bank%):ENDPROC
 1570IF mc%=2 PROCselectbank:?(rombuf%+&21B)=1:PROCmove(bank%,1):?(rombase%+romslot%)=&82:ENDPROC:REM brkchk+1
 1580PROCselectbank:PROCmaster(bank%)
 1590ENDPROC
 1600:
 1610DEFPROCselectbank
 1620I%=15
 1630maxbank%=-1
 1640REPEAT
 1650IF ram%(I%) THEN maxbank%=I%
 1660I%=I%-1
 1670UNTILmaxbank%=I%+1 OR I%=-1
 1680IF ram%(maxbank%) THEN 1690 ELSE PROCgetbank:GOTO1760
 1690CLS
 1700PROCcen("Load Handirom into bank "+STR$(maxbank%)+" ?",3)
 1710I=0:REPEAT
 1720PROCcen(CHR$131+yesno$(I),13):A%=FNpause:I=I+1
 1730UNTIL I>1 OR A%
 1740IF I=2AND NOT A%THEN1710
 1750IF I=1 THEN bank%=maxbank% ELSE PROCprompt:PROCgetbank
 1760PROCsrload("$.HROMV2",bank%)
 1770ENDPROC
 1780:
 1790DEFPROCgetbank
 1800CLS
 1810LOCAL minbank%
 1820minbank%=-1
 1830I%=0
 1840REPEAT
 1850IF ram%(I%) THEN minbank%=I%
 1860I%=I%+1
 1870UNTILminbank%=I%-1
 1880PROCcen("Enter sideways RAM bank number",3)
 1890PROCromlist
 1900IF minbank%=maxbank% THEN PROCcen("Only bank "+STR$(minbank%)+" available",16):PROCprompt:I=minbank%+1:GOTO 1960
 1910I=minbank%:REPEAT
 1920PROCcen("BANK"+CHR$131+STR$(I)+" ",16):A%=FNpause:I=I+1
 1930IF I<=maxbank% AND NOT ram%(I) I=I+1:GOTO1930
 1940UNTIL I>maxbank% OR A%
 1950IF I>maxbank% AND NOT A% THEN1910
 1960bank%=I-1:?&FE60=bank%
 1970ENDPROC
 1980:
 1990DEFFNtest
 2000A%=0:X%=&FF
 2010= ((USR(&FFF4) AND &FFFF) DIV &100)
 2020:
 2030DEFPROCsrload(file$,bank%)
 2040CLS
 2050PROCcen("Loading $.HROMV2 into rom slot "+STR$(bank%),8)
 2060OSCLI("LOAD "+file$+" "+STR$~(rombuf%))
 2070ENDPROC
 2080:
 2090DEFPROCmaster(romslot%)
 2100OSCLI("SRWRITE "+STR$~(rombuf%)+"+4000 8000 "+STR$(romslot%))
 2110OSCLI("INSERT "+STR$(romslot%))
 2120OSCLI("SRREAD "+STR$~(buffer%)+"+40 8000 "+STR$(romslot%))
 2130PROCpokeio(rombase%+romslot%,buffer%?6)
 2140ENDPROC
 2150:
 2160DEFPROCbbc(romslot%)
 2170PROCmove(romslot%,1)
 2180?(rombase%+romslot%)=&82
 2190ENDPROC
 2200:
 2210DEFPROCmove(bank%,dir%)
 2220IF dir%=0 ptr2=&70:ptr1=&80 ELSE ptr2=&80:ptr1=&70
 2230P%=code
 2240[OPT 2
 2250 LDA &F4
 2260 PHA
 2270 LDA #bank%
 2280 STA &F4
 2290 STA &FE30
 2300 LDY #0
 2310 STY ptr2
 2320 LDA #&80
 2330 STA ptr2+1
 2340 LDA #rombuf% MOD 256
 2350 STA ptr1
 2360 LDA #rombuf% DIV 256
 2370 STA ptr1+1
 2380.loop
 2390 LDA (&70),Y
 2400 STA (&80),Y
 2410 INY
 2420 BNE loop
 2430 INC &81
 2440 INC &71
 2450 LDA ptr2+1
 2460 CMP #&C0
 2470 BNE loop
 2480 PLA
 2490 STA &F4
 2500 STA &FE30
 2510 RTS
 2520]
 2530CALL code
 2540ENDPROC
 2550:
 2560DEFPROCromlist
 2570FOR I%=0 TO 15
 2580IF I%>7VDU31,20,I%-3 ELSE VDU31,0,I%+5
 2590I$=STR$(I%):IF LENI$=1 I$="0"+STR$(I%)
 2600IF ram%(I%) THEN PRINTI$;CHR$129;"RAM "; ELSE PRINTI$;CHR$134;
 2610IF I%?&2A1 ROM=I%:PROCroms ELSE PRINTCHR$127;CHR$131;"empty";CHR$135
 2620NEXT
 2630ENDPROC
 2640:
 2650DEFPROCroms
 2660A%=&8009:F%=0
 2670REPEAT byte%=FNbyte(A%)
 2680IF byte%>31 AND byte%<127 VDU byte%
 2690A%=A%+1
 2700IF byte%=0 F%=F%+1:PRINT" ";
 2710UNTIL F%=1 OR A%>&8018
 2720PRINTCHR$135;
 2730ENDPROC
 2740DEF FNbyte(B%)
 2750!&F6=B%:Y%=ROM
 2760=USR(&FFB9)AND &FF
 2770:
 2780DEFFNramtest(slot%)
 2790FOR T%=0 TO 2 STEP2
 2800P%=&C00
 2810[OPT T%
 2820LDA &F4
 2830PHA
 2840LDA #slot%
 2850STA &F4
 2860STA &FE30
 2870LDA &8000
 2880EOR #&FF
 2890STA &8000
 2900CMP &8000
 2910PHP
 2920EOR #&FF
 2930STA &8000
 2940PLP
 2950BEQ noram
 2960PLA
 2970STA &F4
 2980STA &FE30
 2990LDA #0
 3000RTS
 3010.noram
 3020PLA
 3030STA &F4
 3040STA &FE30
 3050LDA #&FF
 3060RTS
 3070]
 3080NEXT
 3090=USR(&C00)AND &FF
 3100:
 3110DEFPROCpokeio(addr%,byte%)
 3120!osbuff%=addr%
 3130osbuff%?4=byte%
 3140Y%=osbuff% DIV &100
 3150X%=osbuff% MOD &100
 3160A%=6
 3170CALL &FFF1
 3180ENDPROC
 3190:
 3200CLS
 3210*DIR $
 3220PROCcen("Would you like to save",3)
 3230PROCcen("this configuration?",5)
 3240I=0:REPEAT
 3250PROCcen("SAVE"+CHR$131+yesno$(I),13):A%=FNpause:I=I+1
 3260UNTIL I>1 OR A%
 3270IF I=2AND NOT A%THEN3240
 3280IF I<>1 THEN 3490
 3290PROCprompt
 3300ONERRORGOTO3480
 3310CLS
 3320PROCcen("Insert a blank formatted disc",3)
 3330PROCcen("into the current drive",5)
 3340PROCprompt
 3350PROCmove(bank%,0)
 3360OSCLI"SAVE HCONFIG "+STR$~(rombuf%)+"+4000 D9CD 8000"
 3370VDU21
 3380*SPOOL !BOOT
 3390PRINT"*BASIC"
 3400PRINT"*SRLOAD Hconfig 8000 ";bank%
 3401PRINT"?(&2A1+";bank%;")=&82"
 3410PRINT"*HON"
 3420PRINT"*FX151,78,127"
 3430PRINT"CALL !-4"
 3440*SPOOL
 3450*OPT 4,3
 3460VDU6
 3470A%=&7D:CALL&FFF4
 3480IF ERR=17 THEN PROCdetails ELSE REPORT:END
 3490VDU23,1,1;0;0;0;
 3500*HON
 3510*FX151,78,127
 3520CALL !-4
 3530END
 3540:
 3550DEFPROCdetails
 3560CLS
 3570PROCcen("Now you may need to copy the program",3)
 3580PROCcen("called SRLOAD onto your new disc",4)
 3590PROCcen("Please consult your Handirom manual",6)
 3600PROCcen("for further details",7)
 3610PROCprompt:ENDPROC

