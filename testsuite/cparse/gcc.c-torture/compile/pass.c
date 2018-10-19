/* { dg-require-effective-target untyped_assembly } */
int
foo (a, b, c)
{
  return a + b + c;
}

int
bar ()
{
  int q, w, e, r, t, y;

  return foo ((int) & q, q, w, e, q, (int) &w);
}

/* cp-out: warning: [^:]*: line 13, columns 9-12: wrong number of arguments to function invocation */
