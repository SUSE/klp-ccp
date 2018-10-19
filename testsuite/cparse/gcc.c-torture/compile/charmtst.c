c_move_tst (char b)
{
  char a;

  a = b;
  b = 'b';
  foo (a);
  foo (b);
  foo (a);
  bar (a, b);
  b = a;
  if (b == 0)
    a++;
  return a + b;
}

/* cp-out: warning: [^:]*: line 7, columns 2-5: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 8, columns 2-5: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 9, columns 2-5: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 10, columns 2-5: identifier "bar" not declared */
