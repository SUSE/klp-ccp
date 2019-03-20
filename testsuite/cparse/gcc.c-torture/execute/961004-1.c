int k = 0;

main()
{
  int i;
  int j;

  for (i = 0; i < 2; i++)
    {
      if (k)
	{
	  if (j != 2)
	    abort ();
	}
      else
	{
	  j = 2;
	  k++;
	}
    }
  exit (0);
}

/* cp-out: warning: [^:]*: line 13, column 5: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 21, column 2: identifier "exit" not declared */
