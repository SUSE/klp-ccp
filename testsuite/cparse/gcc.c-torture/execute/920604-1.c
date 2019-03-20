long long
mod (a, b)
     long long a, b;
{
  return a % b;
}

int
main ()
{
  mod (1LL, 2LL);
  exit (0);
}

/* cp-out: warning: [^:]*: line 12, column 2: identifier "exit" not declared */
