# 
# Makefile for the example program
# please note that the file containing main() must be the last one
# specified in the link process

CC       = csc
OBJECTS  = mchopstk.obj mconsole.obj asyn8250.obj philos.obj
SCFLAGS  = -m -a -p

philos.exe: $(OBJECTS)
    link $(OBJECTS), philos, philos, clib.lib csc.lib;

.SUFFIXES
.asm.obj:
    masm $<;

.c.asm:
    $(CC) $< $(SCFLAGS)

