foo (a)
{
  bar (a);
  bar (a);
}

/* cp-out: warning: [^:]*: line 3, column 2: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 4, column 2: identifier "bar" not declared */
