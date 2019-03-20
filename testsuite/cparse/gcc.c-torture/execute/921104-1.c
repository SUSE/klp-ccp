main ()
{
  unsigned long val = 1;

  if (val > (unsigned long) ~0)
    abort();
  exit (0);
}

/* cp-out: warning: [^:]*: line 6, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 7, column 2: identifier "exit" not declared */
