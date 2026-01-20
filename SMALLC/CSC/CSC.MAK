# 
# Makefile for the Concurrent Small C Compiler
# please note that the file containing main() must be the last one
# specified in the link process

CC       = csc
OBJECTS  = cc2.obj cc3.obj cc4.obj cc1.obj
SCFLAGS  = -m -a -p

csc.exe: $(OBJECTS)
    link $(OBJECTS) , csc, csc, clib.lib csc.lib;

.SUFFIXES

.asm.obj:
    masm $<;

.c.asm:
    $(CC) $< $(SCFLAGS)

