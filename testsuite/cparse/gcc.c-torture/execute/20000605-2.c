struct F { int i; };

void f1(struct F *x, struct F *y)
{
  int timeout = 0;
  for (; ((const struct F*)x)->i < y->i ; x->i++)
    if (++timeout > 5)
      abort ();
}

main()
{
  struct F x, y;
  x.i = 0;
  y.i = 1;
  f1 (&x, &y);
  exit (0);
}

/* cp-out: warning: [^:]*: line 8, columns 6-11: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 17, columns 2-6: identifier "exit" not declared */
