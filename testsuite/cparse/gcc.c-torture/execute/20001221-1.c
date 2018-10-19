int main ()
{
  unsigned long long a;
  if (! (a = 0xfedcba9876543210ULL))
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 5, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 6, columns 2-6: identifier "exit" not declared */
