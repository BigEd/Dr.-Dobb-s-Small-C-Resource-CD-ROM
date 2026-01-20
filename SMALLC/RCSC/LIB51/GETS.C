/*
** Gets an entire string from stdin (excluding its newline
** terminator) or size-1 characters, whichever comes
** first. The input is terminated by a null character.
** The user buffer must be large enough to hold the data.
** Entry: str  = Pointer to destination buffer.
** Returns str on success, else NULL.
*/
gets(str) char *str; {
  return (_gets(str, 32767, stdin, 0));
  }

_gets(str, size, fd, nl) char *str; unsigned size, fd, nl; {
  int backup; char *next;
  next = str;
  while(--size > 0) {
    switch (*next = getchar()) {
      case  EOF: *next = NULL;
                 if(next == str) return (NULL);
                 return (str);
      case '\n': *(next + nl) = NULL;
                 return (str);
      case  RUB: if(next > str) backup = 1; else backup = 0;
                 goto backout;
      case WIPE: backup = next - str;
        backout: /*if(iscons(fd)) {*/
                   ++size;
                   while(backup--) {
                     fputs("\b \b", stderr);
                     --next; ++size;
                     }
                   continue;
                   /*}*/
        default: ++next;
      }
    }
  *next = NULL;
  return (str);
  }

