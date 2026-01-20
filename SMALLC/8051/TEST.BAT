rem Recompile & assemble test file
cc51 %1 -l1 -p <test.c >test.asm
if errorlevel 1 goto exit
..\ASM\a51 test
:exit
