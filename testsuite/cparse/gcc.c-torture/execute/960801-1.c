unsigned
f ()
{
  long long l2;
  unsigned short us;
  unsigned long long ul;
  short s2;

  ul = us = l2 = s2 = -1;
  return ul;
}

unsigned long long
g ()
{
  long long l2;
  unsigned short us;
  unsigned long long ul;
  short s2;

  ul = us = l2 = s2 = -1;
  return ul;
}

main ()
{
  if (f () != (unsigned short) -1)
    abort ();
  if (g () != (unsigned short) -1)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 28, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 30, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 31, columns 2-6: identifier "exit" not declared */
