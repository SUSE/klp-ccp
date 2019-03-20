long long
foo (a, b)
     long long a, b;
{
  return a * b;
}

main ()
{
  int a = foo ((long long) 2, (long long) 3);
  printf ("%d\n", a);
}

/* cp-out: warning: [^:]*: line 11, column 2: identifier "printf" not declared */
