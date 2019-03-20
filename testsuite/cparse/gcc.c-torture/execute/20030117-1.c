int foo (int, int, int);
int bar (int, int, int);

int main (void)
{
  if (foo (5, 10, 21) != 12)
    abort ();

  if (bar (9, 12, 15) != 150)
    abort ();

  exit (0);
}

int foo (int x, int y, int z)
{
  return (x + y + z) / 3;
}

int bar (int x, int y, int z)
{
  return foo (x * x, y * y, z * z);
}

/* cp-out: warning: [^:]*: line 7, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 10, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 12, column 2: identifier "exit" not declared */
