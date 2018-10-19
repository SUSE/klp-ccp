unsigned long
sub (int a)
{
  return ((0 > a - 2) ? 0 : a - 2) * sizeof (long);
}

main ()
{
  if (sub (0) != 0)
    abort ();

  exit (0);
}

/* cp-out: warning: [^:]*: line 10, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 12, columns 2-6: identifier "exit" not declared */
