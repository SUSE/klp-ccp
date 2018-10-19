foo (a, b)
{
  unsigned x = 1;

  a += b;
  a += x;
  if (a <= 0)
    return 1;
  return 0;
}

main ()
{
  printf ("%d\n", foo (1, ~0));
  printf ("%d\n", foo (0, ~0));
  printf ("%d\n", foo (-1, ~0));
}

/* cp-out: warning: [^:]*: line 14, columns 2-8: identifier "printf" not declared */
/* cp-out: warning: [^:]*: line 15, columns 2-8: identifier "printf" not declared */
/* cp-out: warning: [^:]*: line 16, columns 2-8: identifier "printf" not declared */
