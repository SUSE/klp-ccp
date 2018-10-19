foo (a)
{
  int b;
  do
    {
      b = bar ();
      a = b - 10;
    }
  while (a > 10);
  return a;
}

/* cp-out: warning: [^:]*: line 6, columns 10-13: identifier "bar" not declared */
