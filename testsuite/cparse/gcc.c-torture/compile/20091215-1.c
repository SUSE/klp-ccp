/* { dg-require-effective-target untyped_assembly } */

void bar ();

void
foo (void *x, short y)
{
  bar (x, y + 1);
}

void
bar (x, y)
  void *x;
  char *y;
{
  baz (y);
}

/* cp-out: warning: [^:]*: line 16, columns 2-5: identifier "baz" not declared */
