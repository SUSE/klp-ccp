/* { dg-require-effective-target trampolines } */

main ()
{
  int i = 0;
  int a (int x)
    {
      while (x)
	i++, x--;
      return x;
    }

  if (a (2) != 0)
    abort ();

  exit (0);
}

/* cp-out: warning: [^:]*: line 14, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 16, column 2: identifier "exit" not declared */
