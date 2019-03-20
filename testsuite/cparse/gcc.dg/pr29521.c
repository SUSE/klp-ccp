/* PR 29521 : warning for return with expression in function returning void */
/* { dg-do compile } */
/* { dg-options "" } */

void func (void) { }

void func2 (void)
{
  return func ();
}

void func3 (void)
{
  return 1;  /* { dg-warning "'return' with a value" } */
}

/* cp-out: warning: [^:]*: line 14, columns 2-11: return with value in function returning void */
