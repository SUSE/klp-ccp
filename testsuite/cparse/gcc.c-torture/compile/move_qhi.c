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

/* cp-out: warning: [^:]*: line 7, columns 4-9: identifier "gurka" not declared */
/* cp-out: warning: [^:]*: line 8, columns 2-5: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 9, columns 6-9: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 10, columns 6-9: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 11, columns 2-5: identifier "gra" not declared */

