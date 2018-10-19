long long
f (long long x)
{
  return x / 10000000000LL;
}

main ()
{
  if (f (10000000000LL) != 1 || f (100000000000LL) != 10)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 10, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 11, columns 2-6: identifier "exit" not declared */
