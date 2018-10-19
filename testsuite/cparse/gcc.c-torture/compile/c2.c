foo (a)
{
  bar (a);
  bar (a);
}

/* cp-out: warning: [^:]*: line 3, columns 2-5: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 4, columns 2-5: identifier "bar" not declared */
