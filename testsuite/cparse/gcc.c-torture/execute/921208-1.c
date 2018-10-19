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

/* cp-out: warning: [^:]*: line 16, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 17, columns 2-6: identifier "exit" not declared */
