/* { dg-do compile } */
/* { dg-options "-Wno-int-conversion" } */

int fn1 (int *), *fn2 (int);

int
fn1 (int *p)
{
  int i = p;
  i = p;
  fn2 (p);
  return p;
}

int *
fn2 (int i)
{
  int *p = i;
  p = i;
  fn1 (i);
  return i;
}

/* cp-out: warning: [^:]*: line 9, column 10: assignment to integer from pointer */
/* cp-out: warning: [^:]*: line 10, column 6: assignment to integer from pointer */
/* cp-out: warning: [^:]*: line 11, column 7: assignment to integer from pointer */
/* cp-out: warning: [^:]*: line 12, column 9: assignment to integer from pointer */
/* cp-out: warning: [^:]*: line 18, column 11: assignment to pointer from integer which is not NULL */
/* cp-out: warning: [^:]*: line 19, column 6: assignment to pointer from integer which is not NULL */
/* cp-out: warning: [^:]*: line 20, column 7: assignment to pointer from integer which is not NULL */
/* cp-out: warning: [^:]*: line 21, column 9: assignment to pointer from integer which is not NULL */
