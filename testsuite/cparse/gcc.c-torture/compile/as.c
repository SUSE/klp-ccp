
#define S 31
#define A 17

foo (a)
     unsigned a;
{
  return (a >> S) & ((1 << A) - 1);
}

main ()
{
  printf ("%d%d\n", foo (-1), foo (0));
}

/* cp-out: warning: [^:]*: line 13, columns 2-8: identifier "printf" not declared */
