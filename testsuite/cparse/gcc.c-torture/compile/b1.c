foo (long long x)
{
  if (x--)
    return 255;
  return 0;
}

main ()
{
  printf ("%d\n", foo (0));
}

/* cp-out: warning: [^:]*: line 10, columns 2-8: identifier "printf" not declared */
