/* PR middle-end/38360 */
/* { dg-require-effective-target untyped_assembly } */

int
main ()
{
  fputs ("");
  fputs_unlocked ("");
  return 0;
}

/* cp-out: warning: [^:]*: line 7, column 2: identifier "fputs" not declared */
/* cp-out: warning: [^:]*: line 8, column 2: identifier "fputs_unlocked" not declared */
