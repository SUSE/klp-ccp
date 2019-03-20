int *x;
static void bar (char a[2][(*x)++]) {}
int
main (void)
{
  exit (0);
}

/* cp-out: warning: [^:]*: line 6, column 2: identifier "exit" not declared */
