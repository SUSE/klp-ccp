
int * foo (int *x, int b)
{

  *(x++) = 55;
  if (b)
    *(x++) = b;

  return x;
}

main()
{
  int a[5];

  memset (a, 1, sizeof (a));

  if (foo(a, 0) - a != 1 || a[0] != 55 || a[1] != a[4])
    abort();

  memset (a, 1, sizeof (a));

  if (foo(a, 2) - a != 2 || a[0] != 55 || a[1] != 2)
    abort();

  exit (0);
}

/* cp-out: warning: [^:]*: line 16, column 2: identifier "memset" not declared */
/* cp-out: warning: [^:]*: line 19, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 21, column 2: identifier "memset" not declared */
/* cp-out: warning: [^:]*: line 24, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 26, column 2: identifier "exit" not declared */
