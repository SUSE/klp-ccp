f()
{
  struct { char x; } r;
  g(r);
}

/* cp-out: warning: [^:]*: line 4, column 2: identifier "g" not declared */
