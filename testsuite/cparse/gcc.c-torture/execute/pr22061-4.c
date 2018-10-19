/* { dg-skip-if "requires alloca" { ! alloca } { "-O0" } { "" } } */
void
bar (int N)
{
  void foo (int a[2][N++]) {}
  int a[2][N];
  foo (a);
  int b[2][N];
  foo (b);
  if (sizeof (a) != sizeof (int) * 2 * 1)
    abort ();
  if (sizeof (b) != sizeof (int) * 2 * 2)
    abort ();
  if (N != 3)
    abort ();
}

int
main (void)
{
  bar (1);
  exit (0);
}

/* cp-out: warning: [^:]*: line 11, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 13, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 15, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 22, columns 2-6: identifier "exit" not declared */
