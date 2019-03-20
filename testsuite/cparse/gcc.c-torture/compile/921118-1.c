inline f(i)
{
  h((long long) i * 2);
}
g()
{
  f(9);
}

/* cp-out: warning: [^:]*: line 3, column 2: identifier "h" not declared */
