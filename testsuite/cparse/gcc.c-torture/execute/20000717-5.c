typedef struct trio { int a, b, c; } trio;

int
bar (int i, int j, int k, trio t)
{
  if (t.a != 1 || t.b != 2 || t.c != 3 ||
      i != 4 || j != 5 || k != 6)
    abort ();
}

int
foo (trio t, int i, int j, int k)
{
  return bar (i, j, k, t);
}

main ()
{
  trio t = { 1, 2, 3 };

  foo (t, 4, 5, 6);
  exit (0);
}

/* cp-out: warning: [^:]*: line 8, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 22, columns 2-6: identifier "exit" not declared */
