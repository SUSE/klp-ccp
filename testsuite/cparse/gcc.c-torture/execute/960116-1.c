static inline
p (int *p)
{
  return !((long) p & 1);
}

int
f (int *q)
{
  if (p (q) && *q)
    return 1;
  return 0;
}

main ()
{
  if (f ((int*) 0xffffffff) != 0)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 18, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 19, column 2: identifier "exit" not declared */
