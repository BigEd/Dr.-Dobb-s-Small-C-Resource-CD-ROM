/*
** Allocate n bytes of (possibly zeroed) memory.
** Entry: n = Size of the items in bytes.
**    clear = "true" if clearing is desired.
** Returns the address of the allocated block of memory
** or NULL if the requested amount of space is not available.
*/
extern char * _memptr;
extern char * _memend;

_alloc(n, clear) unsigned n, clear; {
  char *oldptr;
  if(n < avail(YES)) {
   /* need to disable interrupt to provide exclusive access
	in multitasking environment */
    disable();
    oldptr = _memptr;
    _memptr += n;
    enable();
    if(clear) pad(oldptr, NULL, n);
    return (oldptr);
    }
  return (NULL);
  }


/*
** Return the number of bytes of available memory.
** In case of a stack overflow condition, if 'abort'
** is non-zero the program aborts with an 'S' clue,
** otherwise zero is returned.
*/
avail(abort) int abort; {
char *ptr;
   /* need to disable interrupt to provide exclusive access
	in multitasking environment */
  disable();
  if(_memend < _memptr)
    ptr = NULL;
  else
    ptr = (_memend - _memptr + 1);
  enable();

  return ptr;
  }

