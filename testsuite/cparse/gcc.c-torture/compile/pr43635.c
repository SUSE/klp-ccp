/* { dg-require-effective-target untyped_assembly } */
extern void d (void);

void (*foo (void)) (float)
{
  void (*(*x) (void)) (float) = d;
  return (*x) ();
}

/* cp-out: warning: [^:]*: line 6, column 32: incompatible pointer types in assignment */
