void foo ()
{
  int x1, x2, x3;

  bar (&x2 - &x1, &x3 - &x2);
}

/* cp-out: warning: [^:]*: line 5, columns 2-5: identifier "bar" not declared */
