/* { dg-require-effective-target trampolines } */

f (void (*func) ())
{
  func ();
}

main ()
{
  void t0 ()
    {
    }

  void t1 ()
    {
      f (t0);
    }

  void t2 ()
    {
      t1 ();
    }

  t1 ();
  t1 ();
  t2 ();

  exit (0);
}

/* cp-out: warning: [^:]*: line 28, columns 2-6: identifier "exit" not declared */
