/* { dg-require-effective-target trampolines } */

int f1()
{
  { int ___() { foo(1); } bar(___); }
  return( { int ___() { foo(2); } bar(___);} );
}

int f2(int j)
{
  { int ___() { foo(j); } bar(___); }
  return( { int ___() { foo(j); } bar(___);} );
}

/* cp-out: warning: [^:]*: line 5, column 16: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 5, column 26: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 6, column 24: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 6, column 34: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 11, column 16: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 11, column 26: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 12, column 24: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 12, column 34: identifier "bar" not declared */
