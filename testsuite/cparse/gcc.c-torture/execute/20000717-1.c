typedef struct trio { int a, b, c; } trio;

int
bar (int i, trio t)
{
  if (t.a == t.b || t.a == t.c)
    abort ();
}

int
foo (trio t, int i)
{
  return bar (i, t);
}

main ()
{
  trio t = { 1, 2, 3 };

  foo (t, 4);
  exit (0);
}

/* cp-out: warning: [^:]*: line 7, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 21, columns 2-6: identifier "exit" not declared */
