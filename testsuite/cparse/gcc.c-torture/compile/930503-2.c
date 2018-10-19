f()
{
  struct { char x; } r;
  g(r);
}

/* cp-out: warning: [^:]*: line 4, columns 2-3: identifier "g" not declared */
