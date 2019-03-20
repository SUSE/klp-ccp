/* A function pointer compared with a void pointer should not be canonicalized.
   See PR middle-end/17564.  */
void *f (void) __attribute__ ((__noinline__));
void *
f (void)
{
  return f;
}
int
main (void)
{
  if (f () != f)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 13, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 14, column 2: identifier "exit" not declared */
