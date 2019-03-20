void bar (struct S *);
void foo (void *x)
{
  bar ((struct S *) x);
}

/* cp-out: warning: [^:]*: line 4, columns 7-21: incompatible pointer types in assignment */
