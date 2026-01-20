rem Compile and assemble C program
cc51 -l1 -p <%1.c >%1.asm
if errorlevel 1 goto exit
..\ASM\a51 %1
:exit
