/* PR target/8232.  */
/* { dg-require-effective-target untyped_assembly } */

int f (char *p, char *q, int i)
{
  return bcmp (p, q, i);
}

/* cp-out: warning: [^:]*: line 6, columns 9-13: identifier "bcmp" not declared */
