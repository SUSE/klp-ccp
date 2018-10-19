long int a = 100000;
long int b = 21475;

long
f ()
{
  return ((long long) a * (long long) b) >> 16;
}

main ()
{
  if (f () < 0)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 13, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 14, columns 2-6: identifier "exit" not declared */
