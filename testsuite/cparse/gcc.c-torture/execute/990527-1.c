int sum;

void
g (int i)
{
  sum += i;
}

void
f(int j)
{
  int i;

  for (i = 0; i < 9; i++)
    {
      j++;
      g (j);
      j = 9;
    }
}

int
main ()
{
  f (0);
  if (sum != 81)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 27, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 28, columns 2-6: identifier "exit" not declared */
