int
f()
{
  int j = 1;
  long i;
  i = 0x60000000L;
  do
    {
      j <<= 1;
      i += 0x10000000L;
    } while (i < -0x60000000L);
  return j;
}

int
main ()
{
  if (f () != 2)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 19, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 20, column 2: identifier "exit" not declared */
