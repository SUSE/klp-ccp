#define mask  0xffff0000L
#define value 0xabcd0000L

long
foo (long x)
{
  if ((x & mask) == value)
    return x & 0xffffL;
  return 1;
}

int 
main (void)
{
  if (foo (value) != 0 || foo (0) != 1)
    abort ();
  
  exit (0);
}

/* cp-out: warning: [^:]*: line 16, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 18, columns 2-6: identifier "exit" not declared */
