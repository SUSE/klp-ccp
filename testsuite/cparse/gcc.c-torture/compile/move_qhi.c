move (a, b)
     char a, b;
{
  char s;
  s = a;
  if (s)
    gurka (s);
  foo (b, a);
  a = bar ();
  b = bar ();
  gra (s);
}

/* cp-out: warning: [^:]*: line 7, column 4: identifier "gurka" not declared */
/* cp-out: warning: [^:]*: line 8, column 2: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 9, column 6: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 10, column 6: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 11, column 2: identifier "gra" not declared */

