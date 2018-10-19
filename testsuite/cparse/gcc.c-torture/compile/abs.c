foo (a)
{
  return __builtin_abs (a);
}

main ()
{
  printf ("%d %d\n", foo (0x80000000), foo (12));
}

/* cp-out: warning: [^:]*: line 8, columns 2-8: identifier "printf" not declared */
