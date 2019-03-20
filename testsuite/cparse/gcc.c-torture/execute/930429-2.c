int
f (b)
{
  return (b >> 1) > 0;
}

main ()
{
  if (!f (9))
    abort ();
  if (f (-9))
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 10, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 12, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 13, column 2: identifier "exit" not declared */
