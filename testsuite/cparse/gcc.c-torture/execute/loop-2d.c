int a[2];

f (b)
{
  unsigned int i;
  int *p;
  for (p = &a[b], i = b; --i < ~0; )
    *--p = i * 3 + (int)a;
}

main ()
{
  a[0] = a[1] = 0;
  f (2);
  if (a[0] != (int)a || a[1] != (int)a + 3)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 16, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 17, columns 2-6: identifier "exit" not declared */
