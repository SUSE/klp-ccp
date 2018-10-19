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

/* cp-out: warning: [^:]*: line 5, columns 16-19: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 5, columns 26-29: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 6, columns 24-27: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 6, columns 34-37: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 11, columns 16-19: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 11, columns 26-29: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 12, columns 24-27: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 12, columns 34-37: identifier "bar" not declared */
