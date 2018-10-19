/* { dg-require-effective-target label_values } */

struct bp { void *v, *b, *e; };
f ()
{
  struct bp x = { &&L2 };
  if (&&L3 - &&L1 > 1)
    abort ();
L1:return 1;
L2:abort ();
L3:;
}

/* cp-out: warning: [^:]*: line 8, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 10, columns 3-8: identifier "abort" not declared */
