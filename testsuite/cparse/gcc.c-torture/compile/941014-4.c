/* { dg-require-effective-target label_values } */

f (int *re)
{
  int *loops = 0, *loope = 0;
  unsigned dat0 = 0;
  static void *debug = &&firstdebug;

 firstdebug:
  g (loops, loope);

  if (dat0 & 1)
    re[(dat0 >> 2) & 3] = 0;
}

/* cp-out: warning: [^:]*: line 10, columns 2-3: identifier "g" not declared */
