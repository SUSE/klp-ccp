void
foo (int n, int m)
{
  if (m == 0)
    exit (0);
  else if (n != 0)
    abort ();
  else
    foo (n++, m - 1);
}
 
int
main (void)
{
  foo (0, 4);
}

/* cp-out: warning: [^:]*: line 5, column 4: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 7, column 4: identifier "abort" not declared */
