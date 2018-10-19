int x, y;

int
main()
{
  x = 2;
  y = x;
  do
    {
      x = y;
      y = 2 * y;
    }
  while ( ! ((y - x) >= 20));
  exit (0);
}

/* cp-out: warning: [^:]*: line 14, columns 2-6: identifier "exit" not declared */
