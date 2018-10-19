/* { dg-skip-if "requires untyped assembly" { ! untyped_assembly } { "-O0" } { "" } } */

int foo () {}

main (a, b)
{
  foo (foo (a, b), foo (b, a));
  return 0;
}

/* cp-out: warning: [^:]*: line 7, columns 7-10: wrong number of arguments to function invocation */
/* cp-out: warning: [^:]*: line 7, columns 19-22: wrong number of arguments to function invocation */
/* cp-out: warning: [^:]*: line 7, columns 2-5: wrong number of arguments to function invocation */
