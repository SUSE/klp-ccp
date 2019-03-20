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

/* cp-out: warning: [^:]*: line 6, column 10: identifier "bar" not declared */
