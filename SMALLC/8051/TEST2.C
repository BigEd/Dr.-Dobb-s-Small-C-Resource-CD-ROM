#include clib\common51.c
#include clib\syslib51.c

char hexdig[]="0123456789ABCDEF";
puthex(n) unsigned int n; {
    _putkey(' ');
    _putkey(hexdig[(n >> 12) & 0xF]);
    _putkey(hexdig[(n >> 8)  & 0xF]);
    _putkey(hexdig[(n >> 4)  & 0xF]);
    _putkey(hexdig[n & 0xF]);
}

putdec(n) int n; {
    unsigned int u,i;
    char c[8],f;
    if (n<0) { f='-'; u=-n; }
    else     { f=0; u=n; }
    i=0;
    do {
        c[i++] = (u%10) + 48;
        u=u/10;
    } while(u);
    _putkey(' ');
    if(f) _putkey(f);
    while(i) _putkey(c[--i]);
}

puts(s) char *s; {
    char c;
    while (c=*s++) {
        if (c==10) _putkey(13);
        _putkey(c);
    }
}
error(s) char *s; {
    puts("\nError: ");
    puts(s);
}

/* test code */

int gi, *gip;   /* globals */
unsigned int gui;
char gc, *gcp;
unsigned char guc;

test(p1,p2)
int p1,p2;      /* parameters */
{
    int li, *lip;   /* locals */
    unsigned int lui;
    char lc, *lcp;
    unsigned char luc;

    puts("\nSmall C Test");
    putdec(1234); putdec(-1234); puthex(0x1234);
    li=-100;  /* p1=100 */
    gi=-30;
    if ((p1+gi) !=  70) error("+");
    if ((p1-gi) != 130) error("-");
    if ((-li) !=   100)  error("-n");
    if ((li*gi) != 3000) error("*");
    if ((((li/gi)*gi) + (li%gi)) != li) error("/1");
    if ((((p1/gi)*gi) + (p1%gi)) != p1) error("/2");
    gi=30;
    if ((((li/30)*30) + (li%30)) != li) error("/3");
    if ((((p1/gi)*gi) + (p1%gi)) != p1) error("/4");
    gui=gi; /* 30 */
    lui=p1; /* 100 */
    if ((gi++) != 30) error("g++");
    if ((++gi) != 32) error("++g");
    if ((gi--) != 32) error("g--");
    if ((--gi) != 30) error("--g");
    if ((li--) != -100) error("l--");
    if ((--li) != -102) error("--l");
    if ((li++) != -102) error("l++");
    if ((++li) != -100) error("++l");
    li += 50;  if (li != -50) error ("+=");
    li -= -10; if (li != -40) error ("-=");
    li /= 2;   if (li != -20) error ("/=");
    li *= -5;  if (li != 100) error ("*=");
    li >>= 2;  if (li !=  25) error (">>=");
    li &= 41;  if (li != 9) error ("&=");
    li <<= 2;  if (li != 36) error ("<<=");
    li |=  5;  if (li != 37) error ("|=");
    puts("\nEnd test.");
    return(1);
}

main() {
    int x1;
    while(1) {
        puts("\nswitch test ");
        switch(_getkey()) {
            case 'a': puts("one");  break;
            case 'b': puts("two");  break;
            case 'c': puts("three");  break;
            default: puts("what?");
        }
        puts(" ...ends");
    }
    x1=100;
    while(test(x1,30)) _putkey(_getkey());
}
