struct foo { int a, b, c; };

void
brother (int a, int b, int c)
{
  if (a)
    abort ();
}

void
sister (struct foo f, int b, int c)
{
  brother ((f.b == b), b, c);
}

int
main ()
{
  struct foo f = { 7, 8, 9 };
  sister (f, 1, 2);
  exit (0);
}

/* cp-out: warning: [^:]*: line 7, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 21, columns 2-6: identifier "exit" not declared */
