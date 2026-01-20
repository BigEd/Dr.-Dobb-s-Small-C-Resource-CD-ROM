   10 REM >source
   20 
   30 REM ******************************************
   40 REM *                                        *
   50 REM * The User Friendly Printer Driver V 1.0 *
   60 REM *                                        *
   70 REM *       (c) 1991 by Ian Grimstead        *
   80 REM *         email: ijg@uk.ac.cf.cm         *
   90 REM * Snail mail: 55 Frenchay Close, Downend *
  100 REM *             Bristol, Avon, BS16 2QJ    *
  110 REM *                                        *
  120 REM ******************************************
  130 
  140 MODE 7
  150 code=&900 : REM The start address of the code
  160   170 left$="[" : REM This is the symbol indicating the start of a command
  180 right$="]": REM This is the symbol indicating the end of a command
  190 fname$="DRIVER" : REM The filename that the printer driver will be saved with
  200 max_length=12 : REM This is the maximum length of any printer command
  210 
  220 PROCinit  : REM Initialise any variables required
  230 PROCmcode : REM Assemble the machine code
  240 
  250 REM And save it to disc ...
  260 OSCLI("SAVE "+fname$+" "+STR$~code+" "+STR$~P%+" "+STR$~exec)
  270 
  280 PRINT
  290 END
  300 
  310 DEF PROCinit
  320 oswrch=&FFEE : REM address of OS Read CHaracter
  330 insv=&22A    : REM The "INSert into buffer" Vector
  340 possible=2   : REM 
  350 ENDPROC
  360 
  370 DEF PROCmcode
  380 FOR pass=0 TO 2 STEP 2
  390 P%=code
  400 
  410 REM This section reads in all of the Printer Commands, & stores them
  420 REM directly into memory, starting at <code%>.
  430 REM NB. All printer commands MUST fit into 1 page of memory (256 bytes)
  440 REM otherwise the program will crash !! (or, rather, produce an error
  450 REM message & halt assembly). The code could be altered to cope with
  460 REM more than 1 page of memory, but this seemed unnecessary.
  470 
  480 REM Also note that each printer command must be smaller than OR equal to
  490 REM the maximum length allowed; = <max=length>
  500 
  510 RESTORE
  520 REPEAT
  530 READ com$
  540 IF LEN(com$)>max_length THEN PRINT'"Error : ";com$;" more than ";max_length;" chars max size !!!"':END
  550 PROCadd_command
  560 UNTIL com$="*"
  570 
  580 IF pass>1 AND P%>(code+&100) THEN PRINT '"Data for codes must fit into 1 page"'"of memory; codes are ";P%-&100-code;" bytes too long;"'"try shortening some of the command"'"names."'':END
  590 
  600 [ OPT pass \ Set aside room for variables required
  610.status :]:P%=P%+1
  620[ OPT pass:.offset :]:P%=P%+1
  630[ OPT pass:.temp :]:P%=P%+1
  640[ OPT pass:.old_insv :]:P%=P%+2
  650[ OPT pass:.buffer :]:P%=P%+max_length
  660[ OPT pass:.register :]:P%=P%+4
  670 
  680[ OPT pass  \ Called initially when program is run; redirects the
  690.exec \ "Insert Into Buffer" vector to point to this program.
  700 LDA insv:STA old_insv
  710 LDA insv+1:STA old_insv+1
  720 
  730 LDA #new_insv MOD 256:STA insv
  740 LDA #new_insv DIV 256:STA insv+1
  750 
  760 LDA #0:STA status
  770 RTS
  780 
  790.new_insv  \ This routine is now called instead of Insert Into Buffer
  800 STA register+1
  810 PHP:PLA:STA register \ Called to insert into Printer Buffer ???
  820 CPX #3:BNE recall2 \ If not, exit to normal Insert Into Buffer
  830 STX register+2
  840 STY register+3
  850 
  860 LDX register+1
  870 
  880 LDA status \ Are we currently in the middle of parsing a command ??
?
  890 AND #possible \ If we are, goto the routine to see if it is a comman
  900 BNE check_for_com \ recognised by the program )ie. entered by the user)
  910 
  920 CPX #ASC(left$) \ Is the character to be put into the printer buffer the
  930 BNE recall_regs \ "Start Of Command" character ??? If not, exit.
  940 
  950 LDA status:ORA #possible:STA status \ If it is, set "Possible Command"
  960 LDA #0:STA register+1:STA offset \ flag, & blank Character.
  970 
  980.recall_regs  \ Recall registers to their original values that they had
  990 LDY register+3 \ when the routine was called
 1000 LDX register+2
 1010.recall2
 1020 LDA register:PHA
 1030 LDA register+1
 1040 PLP
 1050 JMP (old_insv) \ Jump to the original Insert Into Buffer routine as if
 1060                \ nothing has happened.
 1070 
 1080.no_command \ Used to print out the characters that have been read in so
 1090            \ far, when it is discovered that they are not a command.
 1100 LDA #ASC(left$) \ Print the "Start Command" character, by jumping to the
 1110 STA register+1  \ code that restores the registers & returns control to
 1120 JSR recall_regs \ the original "Insert Into Buffer" routine.
 1130 LDX #0
 1140.nc_loop \ Also print out the rest of the command that has been read in
 1150 STX temp
 1160 LDA buffer,X
 1170 STA register+1:JSR recall_regs
 1180 LDX temp
 1190 INX
 1200 CPX offset
 1210 BNE nc_loop \ If all chars have been printed, then the code
 1220             \ "falls through" into the reset routine below
 1230 
 1240.reset \ This cancels the "Possible Command" flag, & exits.
 1250 LDA#0:STA register+1
 1260 LDA status:AND #255-possible:STA status
 1270 JMP recall_regs
 1280 
 1290.check_for_com \ Possibility of a command being entered
 1300 LDY offset \ Store the latest character into the buffer area ready
 1310 LDA register+1 \ for checking when a complete command has been entered
 1320 STA buffer,Y
 1330 INY
 1340 STY offset
 1350 
 1360 CMP #ASC(right$) \ If the "End Of Command" character has been entered,
 1370 BEQ check_command \ then check the command for one known.
 1380 
 1390 CPY #max_length \ If the command entered is over the maximum no.of
 1400 BEQ no_command \ characters then it must be incorrect; reject command
 1410 CPX #13:BEQ no_command \ If the command contains a "Return" code, then
 1420                        \ also reject it.
 1430 
 1440 LDA #0 \ Otherwise, all must be well, so carry on accepting characters
 1450 STA register+1:JMP recall_regs \ for checking for a known command
 1460 
 1470.check_command
 1480 \ command is from buffer to buffer+offset-1
 1490 LDX #0
 1500 
 1510.com_loop
 1520 LDA commands,X \ If the 1st character of the command is a "*", then this
 1530 CMP #ASC("*") \ indicates the end of the command list; so reject command
 1540 BEQ no_command
 1550 
 1560 LDY #0 \ Compare the inputted command against the stored one; if the
 1570.inner_loop \ command names don't match, then try the next command
 1580 LDA commands,X \ instead. If they match so far, & the character being
 1590 CMP buffer,Y \ compared is the "End Of Command" character, then accept
 1600 BNE next_command \ the character & go to the control code routine
 1610 CMP #ASC(right$):BEQ matching_com 1620 INY 1630 INX 1640 CPY offset 1650 BNE inner_loop
 1660 
 1670 DEX
 1680 JMP next_command
 1690 
 1700.matching_com \ Commands match - ie. command entered into printer is ok
 1710 INX
 1720 LDA commands,X \ Read in number of control codes stored after command
 1730 STA temp
 1740 INX
 1750 STX offset
 1760 
 1770.send_loop \ Loop through control codes stored after command name
 1780 LDX offset
 1790 LDA commands,X
 1800 INX
 1810 STX offset \ Print by loading into the original copy of the A regsiter, &
 1820 STA register+1:JSR recall_regs \ jumping to the original "Insert Into
 1830 DEC temp \ Buffer" code (via recall registers) to print the codes
 1840 BNE send_loop
 1850 
 1860 JMP reset \ Once all the characters have benn printed, return the routine
 1870 \ to normal by reseting it (ie.is now scanning for more commands).
 1880 
 1890.next_command \ If the commands don't match, then step through the data
 1900 LDA commands,X \ until the rest of the current command has been skipped
 1910 INX \ & then return to the main checking loop,ready for comparing
 1920 CMP #ASC(right$) \ against the next command
 1930 BNE next_command
 1940 
 1950 LDA commands,X
 1960 STA temp
 1970 TXA
 1980 CLC
 1990 ADC temp
 2000 TAX:INX
 2010 JMP com_loop
 2020 
 2030.commands
 2040 ]
 2050 
 2060 NEXT
 2070 ENDPROC
 2080 
 2090 DEF PROCadd_command
 2100 REM This routine reads in the name of the next command in the data
 2110 REM of the program.
 2120 $P%=com$+right$
 2130 P%=P%+LEN(com$)+1
 2140 count=0
 2150 
 2160 REM It then repeatedly reads in the control codes to be associated with
 2170 REM the command, & stores them in memory after the command name.
 2180 REPEAT
 2190 count=count+1
 2200 READ byte
 2210 IF byte<>-1 THEN P%?count=byte
 2220 UNTIL byte=-1
 2230 
 2240 ?P%=count-1
 2250 P%=P%+count
 2260 
 2270 REM On pass 2 or 3 the routine displays the information about the command
 2280 IF pass>1 THEN PRINT;left$;com$;right$;" ";count-1
 2290 ENDPROC
 2300 
 2310 REM Each command consists of :
 2320 REM a string = The Name of the command
 2330 REM a list of integers = the control codes that this command produces
 2340 REM -1 = to indicate the end of the list & the start of the next command
 2350 REM The last command should be named "*"; this is to indicate the end of
 2351 REM NB. These commands are for an EPSON compatible printer !!!
 2360 REM command list.
 2370 DATA "DWIDTH ON",14,-1
 2380 DATA "DWIDTH OFF",20,-1
 2390 DATA "DSTRIKE ON",27,71,-1
 2400 DATA "DSTRIKE OFF",27,72,-1
 2410 DATA "COND.ON",15,-1
 2420 DATA "COND.OFF",18,-1
 2430 DATA "ULINE ON",27,45,1,-1
 2440 DATA "ULINE OFF",27,45,0,-1
 2450 DATA "EMPHAS ON",27,69,-1
 2460 DATA "EMPHAS OFF",27,70,-1
 2470 DATA "PICA ON",27,80,-1
 2480 DATA "PICA OFF",27,64,-1
 2490 DATA "ELITE ON",27,77,-1
 2500 DATA "ELITE OFF",27,64,-1
 2510 DATA "SUPERSC ON",27,83,0,-1
 2520 DATA "SUBSC ON",27,83,1,-1
 2530 DATA "SCRIPT OFF",27,84,-1
 2540 DATA "*",-1
