void
fn_4parms (unsigned char a, long *b, long *c, unsigned int *d)
{
  if (*b != 1 || *c != 2 || *d != 3)
    abort ();
}

int
main ()
{
  unsigned char a = 0;
  unsigned long b = 1, c = 2;
  unsigned int d = 3;

  fn_4parms (a, &b, &c, &d);
  return 0;
}

/* cp-out: warning: [^:]*: line 5, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 15, columns 16-17: pointers to different integer types in assignment */
/* cp-out: warning: [^:]*: line 15, columns 20-21: pointers to different integer types in assignment */
