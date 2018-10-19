foo (a)
{
  return (a & (1 << 31)) != 0;
}

main ()
{
  if (foo (0))
    puts ("foo");
  else
    puts ("bar");
  if (foo (~0))
    puts ("foo");
  else
    puts ("bar");
}

/* cp-out: warning: [^:]*: line 9, columns 4-8: identifier "puts" not declared */
/* cp-out: warning: [^:]*: line 11, columns 4-8: identifier "puts" not declared */
/* cp-out: warning: [^:]*: line 13, columns 4-8: identifier "puts" not declared */
/* cp-out: warning: [^:]*: line 15, columns 4-8: identifier "puts" not declared */
/* cp-out: warning: [^:]*: line 3, columns 15-16: integer overflow in shift operation */
