f (int i)
{
  if (((1 << i) & 1) == 0)
    abort ();
}

main ()
{
  f (0);
  exit (0);
}

/* cp-out: warning: [^:]*: line 4, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 10, column 2: identifier "exit" not declared */
