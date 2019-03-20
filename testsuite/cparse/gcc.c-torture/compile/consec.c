/* { dg-require-effective-target untyped_assembly } */
int glob;

conseq (a, b, c, d)
     int *a, *b;
{
  a[2] = d;
  a[1] = c;
  sequence (a, b, c, d);
  sequence (d, c, b, a);
  b[0] = 0;
  b[1] = 123;
  a[0] = 321;
  a[1] = 0;
  sequence (111, 0, 0, 222, 0, 333);
  ((int *)glob)[2] = c;
  ((int *)glob)[3] = d;
}

/* cp-out: warning: [^:]*: line 9, column 2: identifier "sequence" not declared */
/* cp-out: warning: [^:]*: line 10, column 2: identifier "sequence" not declared */
/* cp-out: warning: [^:]*: line 15, column 2: identifier "sequence" not declared */
