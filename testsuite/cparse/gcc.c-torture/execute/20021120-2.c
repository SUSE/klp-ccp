int g1, g2;

void foo (int x)
{
  int y;

  if (x)
    y = 793;
  else
    y = 793;
  g1 = 7930 / y;
  g2 = 7930 / x;
}

int main ()
{
  foo (793);
  if (g1 != 10 || g2 != 10)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 19, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 20, column 2: identifier "exit" not declared */
