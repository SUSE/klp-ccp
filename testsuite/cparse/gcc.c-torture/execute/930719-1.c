int
f (foo, bar, com)
{
  unsigned align;
  if (foo)
    return 0;
  while (1)
    {
      switch (bar)
	{
	case 1:
	  if (com != 0)
 	    return align;
	  *(char *) 0 = 0;
	}
    }
}

main ()
{
  f (0, 1, 1);
  exit (0);
}

/* cp-out: warning: [^:]*: line 22, columns 2-6: identifier "exit" not declared */
