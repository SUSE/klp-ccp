int f (i) { return 12; }
int g () { return 0; }

main ()
{
  int i, s;

  for (i = 0; i < 32; i++)
    {
      s = f (i);

      if (i == g ())
	s = 42;
      if (i == 0 || s == 12)
	;
      else
	abort ();
    }

  exit (0);
}

/* cp-out: warning: [^:]*: line 17, column 1: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 20, column 2: identifier "exit" not declared */
