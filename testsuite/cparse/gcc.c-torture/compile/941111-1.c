main ()
{
  struct S { int i; char c; } obj1, obj2;

  foo ();
  if (obj1.c != obj2.c)
    bar ();
}

/* cp-out: warning: [^:]*: line 5, columns 2-5: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 7, columns 4-7: identifier "bar" not declared */
