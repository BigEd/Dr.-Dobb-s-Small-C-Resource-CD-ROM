#include clib\common51.c
#include clib\syslib51.c
#include clib\stdio.h
#include clib\abs.c
#include clib\alloc.c
#include clib\atoi.c
#include clib\atoib.c
#include clib\dtoi.c
#include clib\fgetc.c
#include clib\fgets.c
#include clib\fprintf.c
#include clib\fputc.c
#include clib\fputs.c
#include clib\fscanf.c
#include clib\getchar.c
#include clib\is.c
#include clib\isascii.c
#include clib\itoa.c
#include clib\itoab.c
#include clib\itod.c
#include clib\itoo.c
#include clib\itou.c
#include clib\itox.c
#include clib\left.c
#include clib\lexcmp.c
#include clib\otoi.c
#include clib\poll.c
#include clib\putchar.c
#include clib\puts.c
#include clib\reverse.c
#include clib\sign.c
#include clib\strcat.c
#include clib\strchr.c
#include clib\strcmp.c
#include clib\strcpy.c
#include clib\strlen.c
#include clib\strncat.c
#include clib\strncmp.c
#include clib\strncpy.c
#include clib\strrchr.c
#include clib\toascii.c
#include clib\tolower.c
#include clib\toupper.c
#include clib\ungetc.c
#include clib\utoi.c
#include clib\xtoi.c

main() {
    char c, s[80], *str;
    int i;
  while (1) {
    puts("\nTest begins. Type a key:");
    c=fgetc(stdin);
    str = "You typed";
    printf("\n %s %c. Now type a string:",str,c);
    gets(s);
    fputs(s, stdout);
    puts("\n Now type a decimal number:");
    scanf("%d",&i);
    printf("\n b,d,o,u,x: %b %d %o %u %x ",i,i,i,i,i);
    puts("\n Now type an octal number:");
    scanf("%o",&i);
    printf("\n b,d,o,u,x: %b %d %o %u %x ",i,i,i,i,i);
    puts("\n Now type a hex number:");
    scanf("%x",&i);
    printf("\n b,d,o,u,x: %b %d %o %u %x ",i,i,i,i,i);
    puts("\n Now type an unsigned decimal number:");
    scanf("%u",&i);
    printf("\n b,d,o,u,x: %b %d %o %u %x ",i,i,i,i,i);
    puts("\nDone.");
  }
}
