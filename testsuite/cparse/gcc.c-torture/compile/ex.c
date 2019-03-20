/* { dg-require-effective-target untyped_assembly } */

foo (a, b)
{
  if ((a & (1 << b)) == 0)
    return 1;
  return 0;
}

main ()
{
  printf ("%d\n", foo ());
}

/* cp-out: warning: [^:]*: line 12, column 2: identifier "printf" not declared */
/* cp-out: warning: [^:]*: line 12, columns 18-24: wrong number of arguments to function invocation */
