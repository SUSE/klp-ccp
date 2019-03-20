int
sub1 (int i, int j)
{
  typedef int c[i+2];
  int x[10], y[10];

  if (j == 2)
    {
      memcpy (x, y, 10 * sizeof (int));
      return sizeof (c);
    }
  else
    return sizeof (c) * 3;
}

int
main ()
{
  if (sub1 (20, 3) != 66 * sizeof (int))
    abort ();

  return 0;
}

/* cp-out: warning: [^:]*: line 9, column 6: identifier "memcpy" not declared */
/* cp-out: warning: [^:]*: line 20, column 4: identifier "abort" not declared */
