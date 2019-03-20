f ()
{
  int var = 7;

  if ((var/7) == 1)
    return var/7;
  return 0;
}

main ()
{
  if (f () != 1)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 13, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 14, column 2: identifier "exit" not declared */
