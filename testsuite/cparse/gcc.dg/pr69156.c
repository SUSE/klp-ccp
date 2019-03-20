/* PR tree-optimization/69156 */
/* { dg-do compile } */
/* { dg-options "-O1 -fno-tree-ccp" } */

_Bool
foo ()
{

	_Bool (*f) () = __builtin_abs;	/* { dg-warning "initialization of '_Bool \\(\\*\\)\\(\\)' from incompatible pointer type" } */
  return f (0);
}

/* cp-out: error: [^:]*: line 9, column 17: invalid type for assignment target */
