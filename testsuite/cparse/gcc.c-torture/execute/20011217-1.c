int
main()
{
  double x = 1.0;
  double y = 2.0;

  if ((y > x--) != 1)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 8, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 9, column 2: identifier "exit" not declared */
