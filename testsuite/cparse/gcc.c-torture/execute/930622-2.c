long double
ll_to_ld (long long n)
{
  return n;
}

long long
ld_to_ll (long double n)
{
  return n;
}

main ()
{
  long long n;

  if (ll_to_ld (10LL) != 10.0)
    abort ();

  if (ld_to_ll (10.0) != 10)
    abort ();

  exit (0);
}

/* cp-out: warning: [^:]*: line 18, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 21, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 23, column 2: identifier "exit" not declared */
