typedef union
{
  long align;
  struct
    {
      short h, l;
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

/* cp-out: warning: [^:]*: line 28, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 29, columns 2-6: identifier "exit" not declared */
