int
f (int a)
{
  return ((a >= 0 && a <= 10) && ! (a >= 0));
}

main ()
{
  if (f (0))
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 10, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 11, column 2: identifier "exit" not declared */
