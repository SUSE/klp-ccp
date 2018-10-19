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

/* cp-out: warning: [^:]*: line 23, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 24, columns 2-6: identifier "exit" not declared */
