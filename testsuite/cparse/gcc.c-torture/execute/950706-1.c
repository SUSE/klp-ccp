int
f (int n)
{
  return (n > 0) - (n < 0);
}

main ()
{
  if (f (-1) != -1)
    abort ();
  if (f (1) != 1)
    abort ();
  if (f (0) != 0)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 10, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 12, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 14, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 15, column 2: identifier "exit" not declared */
