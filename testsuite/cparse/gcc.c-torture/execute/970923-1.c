int
ts(a)
     int a;
{
  if (a < 1000 && a > 2000)
    return 1;
  else
    return 0;
}

int
tu(a)
     unsigned int a;
{
  if (a < 1000 && a > 2000)
    return 1;
  else
    return 0;
}


main()
{
  if (ts (0) || tu (0))
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 25, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 26, column 2: identifier "exit" not declared */
