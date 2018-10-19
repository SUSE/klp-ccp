unsigned
f1 (x)
{
  return ((unsigned) (x != 0) - 3) / 2;
}

unsigned long long
f2 (x)
{
  return ((unsigned long long) (x != 0) - 3) / 2;
}

main ()
{
  if (f1 (1) != (~(unsigned) 0) >> 1)
    abort ();
  if (f1 (0) != ((~(unsigned) 0) >> 1) - 1)
    abort ();
  if (f2 (1) != (~(unsigned long long) 0) >> 1)
    abort ();
  if (f2 (0) != ((~(unsigned long long) 0) >> 1) - 1)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 16, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 18, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 20, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 22, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 23, columns 2-6: identifier "exit" not declared */
