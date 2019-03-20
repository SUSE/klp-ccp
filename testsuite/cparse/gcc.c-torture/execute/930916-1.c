f (n)
     unsigned n;
{
  if ((int) n >= 0)
    abort ();
}

main ()
{
  unsigned x = ~0;
  f (x);
  exit (0);
}

/* cp-out: warning: [^:]*: line 5, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 12, column 2: identifier "exit" not declared */
