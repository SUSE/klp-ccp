void
f (a, b, m, n)
     unsigned short *a, *b;
     int m, n;
{
  unsigned long acc;
  int qn;
  int j;

  abort ();

  acc = a[m - 1];
  a += 0;
  for (j = qn - 1; j >= 0; j = j - 1)
    {
      acc = (acc << 16) | a[j];
      acc = acc % *b;
    }

  return;
}

/* cp-out: warning: [^:]*: line 10, columns 2-7: identifier "abort" not declared */
