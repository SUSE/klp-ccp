long long
foo (a)
     int a;
{
  return a;
}

main ()
{
  printf ("%d\n", (int) (foo (-1) >> 32));
}

/* cp-out: warning: [^:]*: line 10, column 2: identifier "printf" not declared */
