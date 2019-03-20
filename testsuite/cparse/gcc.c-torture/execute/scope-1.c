int v = 3;

f ()
{
  int v = 4;
  {
    extern int v;
    if (v != 3)
      abort ();
  }
}

main ()
{
  f ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 9, column 6: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 16, column 2: identifier "exit" not declared */
