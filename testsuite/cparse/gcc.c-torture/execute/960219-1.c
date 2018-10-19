f (int i)
{
  if (((1 << i) & 1) == 0)
    abort ();
}

main ()
{
  f (0);
  exit (0);
}

/* cp-out: warning: [^:]*: line 4, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 10, columns 2-6: identifier "exit" not declared */
