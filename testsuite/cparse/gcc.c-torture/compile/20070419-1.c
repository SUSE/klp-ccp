/* PR tree-optimization/31632 */

struct S
{
  long int l;
  void *m;
};

int
foo (struct S *x)
{
  unsigned long a;
  a = x->l;
  if (a <= ((void *) 0))
    x->m = 0;
  return 0;
}

/* cp-out: warning: [^:]*: line 14, column 6: pointer comparison against non-NULL integer */
