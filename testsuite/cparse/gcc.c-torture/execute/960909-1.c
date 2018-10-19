int
ffs (x)
     int x;
{
  int bit, mask;

  if (x == 0)
    return 0;

  for (bit = 1, mask = 1; !(x & mask); bit++, mask <<= 1)
    ;

  return bit;
}

f (x)
     int x;
{
  int y;
  y = ffs (x) - 1;
  if (y < 0) 
    abort ();
}

main ()
{
  f (1);
  exit (0);
}

/* cp-out: warning: [^:]*: line 22, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 28, columns 2-6: identifier "exit" not declared */
