char a[10] = "deadbeef";

char
acc_a (long i)
{
  return a[i-2000000000L];
}

main ()
{
  if (acc_a (2000000000L) != 'd')
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 12, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 13, column 2: identifier "exit" not declared */
