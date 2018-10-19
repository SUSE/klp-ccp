/* { dg-require-effective-target untyped_assembly } */
void
foo (x, fn)
  void (*fn) ();
{
  int a = baz ((void *) 0, x);
  (*fn) (x, 0);
}

void
bar (void)
{
  void *x = 0;
  foo (x);
}

/* cp-out: warning: [^:]*: line 6, columns 10-13: identifier "baz" not declared */
/* cp-out: warning: [^:]*: line 14, columns 2-5: wrong number of arguments to function invocation */
