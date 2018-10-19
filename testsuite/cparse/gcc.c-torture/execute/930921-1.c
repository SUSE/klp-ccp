f (x)
     unsigned x;
{
  return (unsigned) (((unsigned long long) x * 0xAAAAAAAB) >> 32) >> 1;
}

main ()
{
  unsigned i;

  for (i = 0; i < 10000; i++)
    if (f (i) != i / 3)
      abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 13, columns 6-11: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 14, columns 2-6: identifier "exit" not declared */
