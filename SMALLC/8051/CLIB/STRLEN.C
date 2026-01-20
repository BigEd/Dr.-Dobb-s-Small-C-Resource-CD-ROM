/*
** return length of string s (fast version)
*/
strlen(s) char *s; {
  s;            /* get pointer in pr */
#asm
    mov dph,r4      ; pointer in DPTR
    mov dpl,r0
..strl1: movx a,@dptr ; look for null character
    jz ..strl2
    inc dptr
    mov a,dph       ; end search when DPTR=0000 (just in case)
    orl a,dpl
    jnz ..strl1
..strl2:        ; DPTR points to null character or = 0.
    mov a,dpl   ; compute DPTR - start adrs, result in pr
    clr c
    subb a,r0
    mov r0,a
    mov a,dph
    subb a,r4
    mov r4,a    ; returns length in pr
#endasm
  }

