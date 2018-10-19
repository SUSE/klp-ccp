f ()
{
  struct { char a, b; } x;
  g (x, x, x, x);
}

/* cp-out: warning: [^:]*: line 4, columns 2-3: identifier "g" not declared */
