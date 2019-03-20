f (char *x)
{
  *x = 'x';
}

main ()
{
  int i;
  char x = '\0';

  for (i = 0; i < 100; ++i)
    {
      f (&x);
      if (*(const char *) &x != 'x')
	abort ();
    }
  exit (0);
}

/* cp-out: warning: [^:]*: line 15, column 1: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 17, column 2: identifier "exit" not declared */
