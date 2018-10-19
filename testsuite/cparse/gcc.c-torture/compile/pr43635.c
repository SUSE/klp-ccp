/* { dg-require-effective-target untyped_assembly } */
extern void d (void);

void (*foo (void)) (float)
{
  void (*(*x) (void)) (float) = d;
  return (*x) ();
}

/* cp-out: warning: [^:]*: line 6, columns 32-33: incompatible pointer types in assignment */
