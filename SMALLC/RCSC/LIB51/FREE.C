/*
** free(ptr) - Free previously allocated memory block.
** Memory must be freed in the reverse order from which
** it was allocated.
** ptr    = Value returned by calloc() or malloc().
** Returns ptr if successful or NULL otherwise.
*/
free(ptr) char *ptr; {
   /* need to disable interrupt to provide exclusive access
	in multitasking environment */
   disable();
   _memptr = ptr;
   enable();
   return ptr;
   }

