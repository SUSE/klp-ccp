main ()
{
  int i;
  int f;

  for (i = 0;; i--)
    {
      f = 0;

      if ((i & (i - 1)) == 0)
	{
	  printf ("d");
	  f = 1;
	}
      if ((i & -i) == i)
	{
	  printf ("t");
	  f = 1;
	}
      if (f)
	printf ("%d\n", i);
    }
}

/* cp-out: warning: [^:]*: line 12, columns 3-9: identifier "printf" not declared */
/* cp-out: warning: [^:]*: line 17, columns 3-9: identifier "printf" not declared */
/* cp-out: warning: [^:]*: line 21, columns 1-7: identifier "printf" not declared */
