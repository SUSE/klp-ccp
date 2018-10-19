/* { dg-require-effective-target trampolines } */

int
g (int a, int b, int (*gi) (int, int))
{
  if ((*gi) (a, b))
    return a;
  else
    return b;
}

f ()
{
  int i, j;
  int f2 (int a, int b)
    {
      return a > b;
    }

  if (g (1, 2, f2) != 2)
    abort ();
}

main ()
{
  f ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 21, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 27, columns 2-6: identifier "exit" not declared */
