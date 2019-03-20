void foo ()
{
  int x1, x2, x3;

  bar (&x2 - &x1, &x3 - &x2);
}

/* cp-out: warning: [^:]*: line 5, column 2: identifier "bar" not declared */
