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

/* cp-out: warning: [^:]*: line 9, column 4: identifier "puts" not declared */
/* cp-out: warning: [^:]*: line 11, column 4: identifier "puts" not declared */
/* cp-out: warning: [^:]*: line 13, column 4: identifier "puts" not declared */
/* cp-out: warning: [^:]*: line 15, column 4: identifier "puts" not declared */
/* cp-out: warning: [^:]*: line 3, columns 15-22: integer overflow in shift operation */
