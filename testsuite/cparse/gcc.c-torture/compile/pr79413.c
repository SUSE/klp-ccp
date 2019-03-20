/* { dg-require-effective-target alloca } */
/* PR c/79413 */

void
foo ()
{
  int a[1/0];
}

void
bar (void)
{
  foo ();
}

/* cp-out: warning: [^:]*: line 7, columns 8-11: division by zero */
