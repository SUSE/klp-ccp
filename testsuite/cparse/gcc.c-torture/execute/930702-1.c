fp (double a, int b)
{
  if (a != 33 || b != 11)
    abort ();
}

main ()
{
  int (*f) (double, int) = fp;

  fp (33, 11);
  f (33, 11);
  exit (0);
}

/* cp-out: warning: [^:]*: line 4, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 13, column 2: identifier "exit" not declared */
