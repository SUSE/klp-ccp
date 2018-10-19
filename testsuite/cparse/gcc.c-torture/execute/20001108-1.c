long long
signed_poly (long long sum, long x)
{
  sum += (long long) (long) sum * (long long) x;
  return sum;
}

unsigned long long
unsigned_poly (unsigned long long sum, unsigned long x)
{
  sum += (unsigned long long) (unsigned long) sum * (unsigned long long) x;
  return sum;
}

int
main (void)
{
  if (signed_poly (2LL, -3) != -4LL)
    abort ();
  
  if (unsigned_poly (2ULL, 3) != 8ULL)
    abort ();

  exit (0);
}

/* cp-out: warning: [^:]*: line 19, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 22, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 24, columns 2-6: identifier "exit" not declared */
