int
main()
{
  double x = 1.0;
  double y = 2.0;

  if ((y > x--) != 1)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 8, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 9, columns 2-6: identifier "exit" not declared */
