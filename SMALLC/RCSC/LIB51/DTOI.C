/*
** dtoi -- convert signed decimal string to integer nbr
**         returns field length, else ERR on error
*/
dtoi(decstr, nbr)  char *decstr;  int *nbr;  {
  int length, s;
  if((*decstr)=='-') {s=1; ++decstr;} else s=0;
  if((length=utoi(decstr, nbr))<0) return ERR;
  if(*nbr<0) return ERR;
  if(s) {*nbr = -*nbr; return ++length;} else return length;
  }

