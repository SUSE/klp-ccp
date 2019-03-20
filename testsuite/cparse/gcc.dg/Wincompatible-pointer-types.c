/* PR c/58286 */
/* { dg-do compile } */
/* { dg-options "-Wno-incompatible-pointer-types" } */

void
fn2 (short *s, long *l)
{
}

unsigned *
fn1 (void)
{
  int (*fpi) (int);
  int (*fpd) (double) = fpi;
  fpi = fpd;
  char *di;
  float *dp = &di;
  di = dp;
  fn2 (dp, di);
  return dp;
}

/* cp-out: warning: [^:]*: line 14, column 24: incompatible pointer types in assignment */
/* cp-out: warning: [^:]*: line 15, column 8: incompatible pointer types in assignment */
/* cp-out: warning: [^:]*: line 17, columns 14-17: incompatible pointer types in assignment */
/* cp-out: warning: [^:]*: line 18, column 7: incompatible pointer types in assignment */
/* cp-out: warning: [^:]*: line 19, column 7: incompatible pointer types in assignment */
/* cp-out: warning: [^:]*: line 19, column 11: assigning pointers to integer types of different width */
/* cp-out: warning: [^:]*: line 20, column 9: incompatible pointer types in assignment */
