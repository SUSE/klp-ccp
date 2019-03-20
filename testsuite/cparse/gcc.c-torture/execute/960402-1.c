f (signed long long int x)
{
  return x > 0xFFFFFFFFLL || x < -0x80000000LL;
}

main ()
{
  if (f (0) != 0)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 9, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 10, column 2: identifier "exit" not declared */
