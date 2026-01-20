/*
** Memory Allocation functions
** includes the old files avail.c, calloc.c, malloc.c, free.c, pad.c,
** plus the function _alloc from csyslib.c.
*/

/*
** Return the number of bytes of available memory.
** In case of a stack overflow condition, if 'abort'
** is non-zero the program aborts with an 'S' clue,
** otherwise zero is returned.
** 8051: the 'abort' option is not supported.
*/
avail(abort) int abort; {
  char x;
  if(&x < _memptr) {
    /* if(abort) exit(1); */
    return (0);
    }
  return (&x - _memptr);
  }

/*
** Place n occurrences of ch at dest.
*/
pad(dest, ch, n) char *dest; unsigned n, ch; {
  while(n--) *dest++ = ch;
  }

/*
** Allocate n bytes of (possibly zeroed) memory.
** Entry: n = Size of the items in bytes.
**    clear = "true" if clearing is desired.
** Returns the address of the allocated block of memory
** or NULL if the requested amount of space is not available.
*/
_alloc(n, clear) unsigned n, clear; {
  char *oldptr;
  if(n < avail(YES)) {
    if(clear) pad(_memptr, NULL, n);
    oldptr = _memptr;
    _memptr += n;
    return (oldptr);
    }
  return (NULL);
  }

/*
** free(ptr) - Free previously allocated memory block.
** Memory must be freed in the reverse order from which
** it was allocated.
** ptr    = Value returned by calloc() or malloc().
** Returns ptr if successful or NULL otherwise.
*/
free(ptr) char *ptr; {
   return (_memptr = ptr);
   }
#asm
.cfree: ljmp .free
#endasm

/*
** Cleared-memory allocation of n items of size bytes.
** n     = Number of items to allocate space for.
** size  = Size of the items in bytes.
** Returns the address of the allocated block,
** else NULL for failure.
*/
calloc(n, size) unsigned n, size; {
  return (_alloc(n*size, YES));
  }

/*
** Memory allocation of size bytes.
** size  = Size of the block in bytes.
** Returns the address of the allocated block,
** else NULL for failure.
*/
malloc(size) unsigned size; {
  return (_alloc(size, NO));
  }

