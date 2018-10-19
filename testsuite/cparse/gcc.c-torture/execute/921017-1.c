/* { dg-skip-if "requires alloca" { ! alloca } { "-O0" } { "" } } */
/* { dg-require-effective-target trampolines } */

f(n)
{
  int a[n];
  int g(i)
    {
      return a[i];
    }
  a[1]=4711;
  return g(1);
}
main()
{
  if(f(2)!=4711)abort();

  exit(0);
}

/* cp-out: warning: [^:]*: line 16, columns 16-21: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 18, columns 2-6: identifier "exit" not declared */
