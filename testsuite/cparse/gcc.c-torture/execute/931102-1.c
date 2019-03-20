typedef union
{
  struct
    {
      char h, l;
    } b;
} T;

f (x)
     int x;
{
  int num = 0;
  T reg;

  reg.b.l = x;
  while ((reg.b.l & 1) == 0)
    {
      num++;
      reg.b.l >>= 1;
    }
  return num;
}

main ()
{
  if (f (2) != 1)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 27, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 28, column 2: identifier "exit" not declared */
