f (x)
{
  if (x != 0 || x == 0)
    return 0;
  return 1;
}

main ()
{
  if (f (3))
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 11, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 12, column 2: identifier "exit" not declared */
