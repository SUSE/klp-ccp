
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

/* cp-out: warning: [^:]*: line 16, columns 2-8: identifier "memset" not declared */
/* cp-out: warning: [^:]*: line 19, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 21, columns 2-8: identifier "memset" not declared */
/* cp-out: warning: [^:]*: line 24, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 26, columns 2-6: identifier "exit" not declared */
