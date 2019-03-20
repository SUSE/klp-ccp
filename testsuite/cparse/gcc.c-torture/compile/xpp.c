foo (a)
{
  a++;
  if (a < 10)
    return 1;
  return a;
}

main ()
{
  printf ("%d\n", foo ((1 << 31) - 1));
}

/* cp-out: warning: [^:]*: line 11, column 2: identifier "printf" not declared */
/* cp-out: warning: [^:]*: line 11, columns 24-31: integer overflow in shift operation */
/* cp-out: warning: [^:]*: line 11, columns 23-36: integer overflow in subtraction */
