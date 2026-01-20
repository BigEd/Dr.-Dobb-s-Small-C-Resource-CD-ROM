echo off
rem ***************************************************
rem create library source file for linking rcsc program
rem parameter 1 contains the path of rcsc
rem parameter 2 contains the info file name
rem ***************************************************
echo #include "%1\src\stdio.h" > tmp
echo #include "%1\lib51\runtime.c" >> tmp
awk  -f %1\bin\genlib.awk -v path=%1\lib51 %2 | sort | uniq >> tmp
fc %2.c tmp | grep "no differences"
if not errorlevel 1 goto match
del %2.c
echo creating/updating %2.c...
ren tmp %2.c
rcsc %2.c
m4 %1\bin\8051.m4 %2.m4 > %2.s
cas -c %2.s
goto end

:match
echo no change in library function requirements

:end

