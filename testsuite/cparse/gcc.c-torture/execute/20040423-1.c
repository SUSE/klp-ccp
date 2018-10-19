int
sub1 (int i, int j)
{
  typedef struct
  {
   int  c[i+2];
  }c;
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
  typedef struct
  {
   int  c[22];
  }c;
  if (sub1 (20, 3) != sizeof (c)*3)
    abort ();

  return 0;
}

/* cp-out: warning: [^:]*: line 12, columns 6-12: identifier "memcpy" not declared */
/* cp-out: warning: [^:]*: line 27, columns 4-9: identifier "abort" not declared */
