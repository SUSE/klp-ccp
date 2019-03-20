int a = 1, b;

g () { return 0; }
h (x) {}

f ()
{
  if (g () == -1)
    return 0;
  a = g ();
  if (b >= 1)
    h (a);
  return 0;
}

main ()
{
  f ();
  if (a != 0)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 20, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 21, column 2: identifier "exit" not declared */
