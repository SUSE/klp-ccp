/* { dg-require-effective-target label_values } */

short optab[5];
char buf[10];
execute (ip)
     register short *ip;
{
  register void *base = &&x;
  char *bp = buf;
  static void *tab[] = {&&x, &&y, &&z};
  if (ip == 0)
    {
      int i;
      for (i = 0; i < 3; ++i)
	optab[i] = (short)(tab[i] - base);
      return;
    }
x:  *bp++='x';
    goto *(base + *ip++);
y:  *bp++='y';
    goto *(base + *ip++);
z:  *bp++='z';
    *bp=0;
    return;
}

short p[5];

main ()
{
  execute ((short *) 0);
  p[0] = optab[1];
  p[1] = optab[0];
  p[2] = optab[1];
  p[3] = optab[2];
  execute (&p);
  if (strcmp (buf, "xyxyz"))
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 37, columns 6-12: identifier "strcmp" not declared */
/* cp-out: warning: [^:]*: line 38, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 39, columns 2-6: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 16, columns 6-12: return without value in function returning non-void */
/* cp-out: warning: [^:]*: line 24, columns 4-10: return without value in function returning non-void */
