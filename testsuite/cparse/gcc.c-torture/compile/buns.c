foo (a)
{
  int bar = 0;

  return (unsigned) (a - 1) <= (unsigned) bar;
}

main ()
{
  if (foo (-1))
    puts ("The largest possible unsigned <= 0 on this machine...");
}

/* cp-out: warning: [^:]*: line 11, column 4: identifier "puts" not declared */
