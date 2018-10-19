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

/* cp-out: warning: [^:]*: line 14, columns 24-27: incompatible pointer types in assignment */
/* cp-out: warning: [^:]*: line 15, columns 8-11: incompatible pointer types in assignment */
/* cp-out: warning: [^:]*: line 17, columns 14-15: incompatible pointer types in assignment */
/* cp-out: warning: [^:]*: line 18, columns 7-9: incompatible pointer types in assignment */
/* cp-out: warning: [^:]*: line 19, columns 7-9: incompatible pointer types in assignment */
/* cp-out: warning: [^:]*: line 19, columns 11-13: assigning pointers to integer types of different width */
/* cp-out: warning: [^:]*: line 20, columns 9-11: incompatible pointer types in assignment */
