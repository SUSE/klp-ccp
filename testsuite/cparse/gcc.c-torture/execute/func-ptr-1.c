static double f (float a);
static double (*fp) (float a);

main ()
{
  fp = f;
  if (fp ((float) 1) != 1.0)
    abort ();
  exit (0);
}

static double
f (float a)
{
  return a;
}

/* cp-out: warning: [^:]*: line 8, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 9, column 2: identifier "exit" not declared */
