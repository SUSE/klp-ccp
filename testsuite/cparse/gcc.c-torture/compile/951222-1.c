extern long long foo ();

long long
sub1 ()
{
  char junk[10000];
  register long long a, b, c;

  b = foo ();

  setjmp ();
  a = foo ();
  c = a - b;
  return c;
}

/* cp-out: warning: [^:]*: line 11, columns 2-8: identifier "setjmp" not declared */
