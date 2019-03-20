main ()
{
  struct S { int i; char c; } obj1, obj2;

  foo ();
  if (obj1.c != obj2.c)
    bar ();
}

/* cp-out: warning: [^:]*: line 5, column 2: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 7, column 4: identifier "bar" not declared */
