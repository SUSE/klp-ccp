main ()
{
  static int a[] = {3, 4};
  register int *b;
  int c;

  b = a;
  c = *b++ % 8;
  if (c != 3)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 10, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 11, columns 2-6: identifier "exit" not declared */
