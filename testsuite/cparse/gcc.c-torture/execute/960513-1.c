long double
f (d, i)
     long double d;
     int i;
{
  long double e;

  d = -d;
  e = d;
  if (i == 1)
    d *= 2;
  d -= e * d;
  d -= e * d;
  d -= e * d;
  d -= e * d;
  d -= e * d;
  return d;
}

main ()
{
  if (! (int) (f (2.0L, 1)))
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 23, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 24, column 2: identifier "exit" not declared */
