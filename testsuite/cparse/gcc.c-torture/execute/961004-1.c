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

/* cp-out: warning: [^:]*: line 13, columns 5-10: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 21, columns 2-6: identifier "exit" not declared */
