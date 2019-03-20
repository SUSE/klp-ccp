main ()
{
  if ((__complex__ double) 0.0 != (__complex__ double) (-0.0))
    abort ();

  if (0.0 != -0.0)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 4, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 7, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 8, column 2: identifier "exit" not declared */
