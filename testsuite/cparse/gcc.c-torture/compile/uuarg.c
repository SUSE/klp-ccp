/* { dg-require-effective-target untyped_assembly } */
foo (a, b, c, d, e, f, g, h, i)
{
  return foo () + i;
}

/* cp-out: warning: [^:]*: line 4, columns 9-15: wrong number of arguments to function invocation */
