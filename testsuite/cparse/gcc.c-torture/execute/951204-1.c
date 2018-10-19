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

/* cp-out: warning: [^:]*: line 15, columns 1-6: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 17, columns 2-6: identifier "exit" not declared */
