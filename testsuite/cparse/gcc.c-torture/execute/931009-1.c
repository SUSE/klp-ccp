main ()
{
  f ();
  exit (0);
}

static
g (out, size, lo, hi)
     int *out, size, lo, hi;
{
  int j;

  for (j = 0; j < size; j++)
    out[j] = j * (hi - lo);
}


f ()
{
  int a[2];

  g (a, 2, 0, 1);

  if (a[0] != 0 || a[1] != 1)
    abort ();
}

/* cp-out: warning: [^:]*: line 3, column 2: identifier "f" not declared */
/* cp-out: warning: [^:]*: line 4, column 2: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 25, column 4: identifier "abort" not declared */
