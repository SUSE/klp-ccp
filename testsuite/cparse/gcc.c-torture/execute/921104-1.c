main ()
{
  unsigned long val = 1;

  if (val > (unsigned long) ~0)
    abort();
  exit (0);
}

/* cp-out: warning: [^:]*: line 6, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 7, columns 2-6: identifier "exit" not declared */
