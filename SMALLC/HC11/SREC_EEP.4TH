( SIMPLE S-RECORD LOADER FOR Max-FORTH V3.5
( INPUT AN S19 RECORD TO HC11 MEMORY( RAM OR EEPROM )
( DOES NOT CHECK CHECKSUM
(
( EDIT TO CHANGE
( LOAD-OFFSET INITILIZATION
( IF OFFSET IS NEEDED
( OTHERWISE LOAD WILL BEGIN
( WITH 0 OFFSET
( JAN. 4, 94

COLD

FORGET TASK

HEX

0000 CONSTANT LOFF ( CAN CHANGE OFFSET IF LOAD ADDR IS NOT RUN ADDR

: GETC
  >IN @ TIB @ + C@  >IN 1+! ;

: 1# ( -- n )
  GETC 30 - DUP 9 > IF 7 - THEN F AND ;

: 2# ( -- nn )
  1# 10 * 1# + ;

: 4# ( -- nnnn )
  2# >< 2# OR ;

: SREC
   CR ." READY TO LOAD, OFFSET = " LOFF U. CR
   0 TIB @ C!                   ( kill the SREC text in TIB
   BEGIN
      QUERY
      GETC 53 = IF              ( only consider things beginning with "S"
         1# 9 = IF              ( type 9 means end
            0 #TIB !            ( clear tib
            EXIT                ( and exit
         THEN                   (
         2# 3 -                 ( len
         4# LOFF +              ( load address
         DUP U.                 ( print for reference
         SWAP OVER + SWAP DO    ( hi lo
            2# I                ( byte and address
            2DUP C!             ( write
            2DUP C@ - IF        ( check
               2DUP EEC!        ( write again
            THEN 2DROP          ( discard
         LOOP
         CR
      THEN
   AGAIN ;

SREC

