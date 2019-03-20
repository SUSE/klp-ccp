/* { dg-do compile } */
/* { dg-options "-O" } */

_Bool foo()
{
  _Bool (*f)(int) = __builtin_abs; /* { dg-warning "" } */
  return f(0);
}

/* cp-out: error: [^:]*: line 6, column 20: invalid type for assignment target */
