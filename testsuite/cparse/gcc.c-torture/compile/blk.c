struct
{
  double a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t;
} foo, bar;

foobar ()
{
  foo = bar;
  xxx (&foo, &bar);
}

main ()
{
  bar.g = 1.0;
  foo.g = 2.0;
  foobar ();
  printf ("%lf\n", foo.g);
}

/* cp-out: warning: [^:]*: line 9, column 2: identifier "xxx" not declared */
/* cp-out: warning: [^:]*: line 17, column 2: identifier "printf" not declared */
