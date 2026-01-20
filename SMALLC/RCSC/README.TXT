This is the Retargetable Concurrent Small C Version 2 distribution.
It contains the RCSC V2 compiler and code generators for the 80x86
and the 8051. The details on how the compiler and the code generators
work are published in the ?month? 1997 issue of Dr. Dobb's Journal. 
An article on the Concurrent Small C Version 1 can be found 
in the August 1996 issue of Dr. Dobb's Journal.

To install RCSC, you should create a directory for it, change to
that directory and unzip the rcsc.zip file as follows:

	c:
	cd\
	md rcsc
	cd rcsc
	pkunzip -d a:rcsc *.*

and add c:\rcsc\bin to your PATH

Note the use of the -d option. The example assumes that the rcsc.zip
file is on your A: drive and that you are installing rcsc on the c:
drive..

This distribution, when unzipped, contains the following directories
and files:

rcsc:
|
+--readme.txt    	the file you are reading
|
+--bin:		contains programs and configuration files
|
+------8051.m4       	m4 macros for the 8051 (for code generation)
+------8086.m4       	m4 macros for the 8086 (for code generation)
+------awk.exe       	awk text processing program
+------cas.exe       	C-assembler (8051 assembler and linker)
+------cat.exe       	UNIX utility cat (DOS version)
+------genlib.awk    	awk program to simulate a 8051 librarian
+------grep.exe      	GNU grep utility
+------header.awk    	awk program to create symbols for code placement
+------m4.exe        	GNU m4 macro processor
+------mklibc.bat    	batch file to create C library functions
+------preproc.awk   	awk program to pre-process 8051 compiler output
+------rcsc.cfg      	8051 memory model configuration file
+------rcsc.exe      	Retargetable Concurrent Small C compiler
+------rm.exe        	UNIX utility rm (DOS version)
+------stdarg51.m4   	ANSI-like standard argument macros for the 8051
+------stdarg86.m4   	ANSI-like standard argument macros for the 80x86
+------uniq.exe      	UNIX utility uniq (DOS version)
|
+--demo51:	demo program for the 8051
|
+------asyn8051.c   
+------kernel.c     
+------makefile     
+------mchopstk.c   
+------mconsole.c   
+------philos.c     
+------philos.hex  	INTEL hex file format demo program 
+------rcsc.cfg     
+------stdio.h      
|
+--demo86:	demo program for the 80x86
|
+------asyn8250.c   
+------makefile     
+------mchopstk.c   
+------mconsole.c   
+------philos.c     
+------philos.exe   
+------stdio.h      
|
+--lib51:	library source for the 8051
|
+------abs.c        
+------atoi.c       
+------atoib.c      
+------avail.c      
+------calloc.c     
+------dtoi.c       
+------file.idx     	function dependency index file
+------free.c       
+------gets.c       
+------is.c         
+------isascii.c    
+------isatty.c     
+------itoa.c       
+------itoab.c      
+------itod.c       
+------itoo.c       
+------itou.c       
+------itox.c       
+------kernel.c     	8051 Concurrent Small C kernel
+------malloc.c     
+------otoi.c       
+------pad.c        
+------puts.c       
+------reverse.c    
+------runtime.c    	8051 RCSC runtime library
+------sign.c       
+------sprintf.c    
+------sprintf.va   
+------sscanf.c     
+------strcat.c     
+------strchr.c     
+------strcmp.c     
+------strcpy.c     
+------strlen.c     
+------strncat.c    
+------strncmp.c    
+------strncpy.c    
+------strrchr.c    
+------utoi.c       
+------xtoi.c       
|
+--lib86:	libraries for the 80x86
|
+------clib.lib    
+------csc.lib     
+------kernel.c    	80x86 Concurrent Small C kernel
+------sprintf.c   
|
+--src:		Retargetable Concurrent Small C compiler source code
|
+------cc.h       
+------cc1.c      
+------cc2.c      
+------cc3.c      
+------cc4.c      
+------clib.h     
+------makefile   
+------notice.h   
+------stdio.h    
|
+--packages:	 software packages (partially installed in ..\bin)
|              
+------awk320.zip     	awk
+------cas.zip        	C-assembler
+------grep15ax.zip   	GNU grep
+------m4v05ax.zip    	GNU m4
+------sc22.zip       	Small C Version 2.2

Note: The makefiles are for PolyMake. You may have to adapt them
before using these makefiles with your make utility.

Have fun!

Andy W. K. Yuen
Jan 1997

email address: andyyuen@ozemail.com.au
