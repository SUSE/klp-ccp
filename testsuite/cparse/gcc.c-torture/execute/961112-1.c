f (x)
{
  if (x != 0 || x == 0)
    return 0;
  return 1;
}

main ()
{
  if (f (3))
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 11, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 12, columns 2-6: identifier "exit" not declared */
