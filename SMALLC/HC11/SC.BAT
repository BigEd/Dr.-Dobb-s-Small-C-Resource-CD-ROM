copy %1.c+6811_lib.c aspo.c
cc aspo.c %2 %3
copy dataout.asm+codeout.asm %1.asm
as11 %1.asm -l >%1.lst
