   10REM Program:  PrintTo
   20REM Author:   (C) Steve Greenley 31/12/85
   30REM Machine:  BBC micro *** with BASIC 2 ***
   40REM Adapted:  Tidied up and initialisation written, Mike Brudenell 28/3/87
   50:
   60REM This program implements a "User Printer Driver" which sends all
   70REM output normally going to a printer to a disk file.
   80:
   90REM Run this program and answer the questions it asks you.  This will
  100REM generate the User Printer Driver routine in a file called "PrintTo".
  110REM To set up the User Printer Driver at any time, run the file as:
  120REM    *PrintTo filename
  130REM where "filename" is the name of the file to which printer output
  140REM is to be redirected.  The code occupies less than 256 bytes of
  150REM memory in the I/O (main BBC micro's) memory.  Select a suitable
  160REM area (for example, the cassette output buffer).
  170:
  180REM Once the User Printer Driver has been set up, using *FX5,3 will
  190REM cause a disk file to be opened.  From then on, all information
  200REM that would normally have gone to the printer will be redirected
  210REM to the disk.  When you have finished using this facility, type
  220REM *FX5,n where n is the number of the printer type you wish to use
  230REM (for the default, parallel, printer n=1)
  240:
  250REM If the micro gives a Beep when the User Printer Driver is selected
  260REM using the *FX5,3 command, the file you specified could not be
  270REM created/emptied for some reason, and the "printer sink" is selected
  280REM (ie. all print output discarded)
  290:
  300:
  310MODE 3
  320:
  330DIM Code% 256
  340:
  350PRINT ''"Enter address where code is to run (I/O processor)."
  360REPEAT
  370INPUT "Address? &" Address$
  380Address% = EVAL("&"+Address$) AND &FFFF
  390UNTIL Address% >= &200
  400:
  410PROCassemble(Address%, Filename$)
  420PROCsave
  430END
  440:
  450:
  460:
  470DEF PROCassemble(startcode%, Filename$)
  480LOCAL Pass%
  490:
  500cblock% = &B0
  510UptVec% = &222
  520osbyte% = &FFF4
  530osfind% = &FFCE
  540osbput% = &FFD4
  550oswrch% = &FFEE
  560osargs% = &FFDA
  570MaxNameLen% = 0
  580:
  590FOR Pass% = 4 TO 7 STEP 3
  600P% = startcode% : O% = Code%
  610[OPT Pass%
  620\
  630\ *RUN entry point to set up the vector to point to this routine
  640.init%
  650JSR getname%     \ Get filename
  660BNE dovector%
  670BRK
  680EQUB 0
  690EQUS "Use:  *PrintTo filename" : EQUB 0
  700\
  710.dovector%
  720PHP
  730SEI              \ Disable interrupts until done
  740LDA #entry% MOD 256    \ Set up the UPT vector
  750STA UptVec%
  760LDA #entry% DIV 256
  770STA UptVec%+1
  780CLI              \ Enable interrupts again
  790PLP
  800RTS
  810\
  820\
  830\
  840\ Main entry.  Check type of activity.
  850.entry%
  860STA temp%        \ Save A
  870PHA : TXA : PHA : TYA : PHA     \ Save A, X and Y registers on stack
  880LDA temp%        \ Recover A
  890BEQ interrupt%   \ Poll "printer" under interrupt .. shouldn't occur
  900CMP #1           \ Is there a character in the printer buffer?
  910BEQ printit%
  920CMP #2           \ Check for a vdu 2
  930BEQ vdu2%
  940CMP #3           \ Check for a vdu 3
  950BEQ vdu3%
  960CMP #5           \ Check for a change in printer type
  970BEQ change%
  980\
  990.exit%
 1000PLA : TAY : PLA : TAX : PLA     \ Recover original A, X and Y
 1010RTS
 1020\
 1030\
 1040\
 1050.interrupt%      \ Poll "printer" under interrupt.
 1060.vdu2%           \ A vdu 2 has been performed
 1070.vdu3%           \ A vdu 3 has been performed
 1080JMP exit%        \ Ignore this .. it shouldn't occur
 1090\
 1100\
 1110\
 1120\ Print a character from the buffer specified by X
 1130.printit%
 1140LDA #145         \ Read the character from buffer number X
 1150LDY #0
 1160JSR osbyte%
 1170JSR send%        \ Send the character to the disk
 1180SEC              \ The "printer" is not going active
 1190JMP exit%
 1200\
 1210\
 1220\
 1230\ The printer type is about to change (close the disk file)
 1240.change%
 1250JSR close%       \ Ensure the receiving file is closed
 1260CPX #3           \ Is the User Printer Driver being selected?
 1270BNE changed%
 1280JSR open%        \ Open the receiving file
 1290CMP #0           \ Was the file opened OK?
 1300BNE changed%
 1310JSR message%     \ Output a failure message
 1320LDA #5           \ Select the printer sink
 1330LDX #0
 1340JSR osbyte%
 1350\
 1360.changed%
 1370JMP exit%
 1380\
 1390\
 1400\
 1410\ Close the file whose handle is in handle%
 1420.close%
 1430LDY handle%
 1440BEQ closed%
 1450LDA #0
 1460STA handle%
 1470JSR osfind%
 1480\
 1490.closed%
 1500RTS
 1510\
 1520\
 1530\
 1540\ Open a file, returning the handle in handle%
 1550.open%
 1560LDA #&80         \ Open the file for output
 1570LDX #name% MOD 256
 1580LDY #name% DIV 256
 1590JSR osfind%
 1600STA handle%
 1610RTS
 1620\
 1630\
 1640\
 1650\ Send a character to the disk file
 1660.send%
 1670TYA
 1680LDY handle%
 1690JSR osbput%
 1700RTS
 1710\
 1720\
 1730\
 1740\ Output a message that the file could not be created
 1750.message%
 1760LDA #7           \ Output a bell
 1770JSR oswrch%
 1780RTS
 1790\
 1800\
 1810\
 1820.getname%
 1830LDA #1           \ Read start of parameters
 1840LDX #cblock%
 1850LDY #0
 1860JSR osargs%
 1870\
 1880.skipspaces%
 1890JSR getch%       \ Get next parameter character
 1900CMP #ASC" "      \ Skip it if it's a space
 1910BEQ skipspaces%
 1920\
 1930LDX #0           \ Point to start of filename
 1940.nextch%
 1950CMP #13          \ Exit if it was a Return
 1960BEQ gotname%
 1970STA name%,X      \ Put character in filename
 1980INX
 1990CPX #MaxNameLen% \ Reached maximum name length?
 2000BEQ gotname%
 2010JSR getch%       \ Get next character of parameters
 2020CMP #ASC" "      \ Exit if it was a space
 2030BEQ gotname%
 2040BNE nextch%
 2050\
 2060.gotname%
 2070LDA #13          \ Terminate name with a Return
 2080STA name%,X
 2090TXA              \ Put filename length in A
 2100RTS
 2110\
 2120\
 2130\
 2140.getch%
 2150LDA (cblock%),Y
 2160INY
 2170RTS
 2180\
 2190\
 2200\
 2210.temp%   EQUB 0
 2220.handle% EQUB 0
 2230.name%   EQUB 13
 2240\
 2250.endcode%
 2260]
 2270MaxNameLen% = 256 - (endcode% - startcode%) - 5
 2280IF (MaxNameLen% <= 0) THEN PRINT "Too much code!" : STOP
 2290NEXT
 2300:
 2310StartAddr% = (startcode% OR &FFFF0000)
 2320CodeLength% = (endcode% - startcode%)
 2330ExecAddr% = (init% OR &FFFF0000)
 2340ENDPROC
 2350:
 2360:
 2370:
 2380DEF PROCsave
 2390LOCAL Cmd$
 2400Cmd$ = "*SAVE PrintTo "+STR$~Code%+" +"+STR$~CodeLength%
 2410Cmd$ = Cmd$+" "+STR$~ExecAddr%+" "+STR$~StartAddr%
 2420PRINT " ";Cmd$
 2430OSCLI Cmd$
 2440ENDPROC
