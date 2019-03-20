/* { dg-require-effective-target alloca } */
int N = 1;
void foo() {} /* Necessary to trigger the original ICE.  */
void bar (char a[2][N]) { a[1][0] = N; }
int
main (void)
{
  void *x;

  N = 4;
  x = alloca (2 * N);
  memset (x, 0, 2 * N);
  bar (x);
  if (N[(char *) x] != N)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 11, column 6: identifier "alloca" not declared */
/* cp-out: warning: [^:]*: line 12, column 2: identifier "memset" not declared */
/* cp-out: warning: [^:]*: line 15, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 16, column 2: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 11, columns 6-20: assignment to pointer from integer which is not NULL */
