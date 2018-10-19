foo (a)
     int a;
{
  int b = 8;

  if ((a << b) >= 0)
    return 1;
  return a;
}

main ()
{
  if (foo (0x00ffffff) == 1)
    puts ("y");
}

/* cp-out: warning: [^:]*: line 14, columns 4-8: identifier "puts" not declared */
