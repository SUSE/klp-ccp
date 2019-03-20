double
f(double x)
{
  return x*x;
}

double
Int(double (*f)(double), double a)
{
  return (*f)(a);
}

main()
{
  if (Int(&f,2.0) != 4.0)
    abort();
  exit (0);
}

/* cp-out: warning: [^:]*: line 16, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 17, column 2: identifier "exit" not declared */
