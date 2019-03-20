int s = 200;
int __attribute__((noinline))
foo (void)
{
  return (signed char) (s - 100) - 5;
}
int
main (void)
{
  if (foo () != 95)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 11, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 12, column 2: identifier "exit" not declared */
