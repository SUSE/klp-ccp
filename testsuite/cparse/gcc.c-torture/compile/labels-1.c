/* { dg-require-effective-target label_values } */

f ()
{
  void *x = &&L2;
  if (&&L3 - &&L1 > 1)
    abort();
 L1: return 1;
 L2: abort ();
 L3:;
}

/* cp-out: warning: [^:]*: line 7, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 9, column 5: identifier "abort" not declared */
