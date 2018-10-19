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

/* cp-out: warning: [^:]*: line 9, columns 6-11: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 16, columns 2-6: identifier "exit" not declared */
