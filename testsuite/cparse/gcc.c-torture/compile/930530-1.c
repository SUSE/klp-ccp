f ()
{
  struct { char a, b; } x;
  g (x, x, x, x);
}

/* cp-out: warning: [^:]*: line 4, column 2: identifier "g" not declared */
