/* { dg-require-effective-target untyped_assembly } */
foo (a, p)
     int *p;
{
  p[0] = a;
  a = (short) a;
  return a;
}

main ()
{
  int i;
  foobar (i, &i);
}


foobar (a, b)
{
  int c;

  c = a % b;
  a = a / b;
  return a + b;
}

/* cp-out: warning: [^:]*: line 13, columns 2-8: identifier "foobar" not declared */
