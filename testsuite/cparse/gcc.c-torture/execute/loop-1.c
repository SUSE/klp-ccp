main ()
{
  int i, j, k[3];

  j = 0;
  for (i=0; i < 3; i++)
    {
      k[i] = j++;
    }

  for (i=2; i >= 0; i--)
    {
      if (k[i] != i)
	abort ();
    }

  exit (0);
}

/* cp-out: warning: [^:]*: line 14, column 1: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 17, column 2: identifier "exit" not declared */
