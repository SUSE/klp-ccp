/* PR middle-end/38360 */
/* { dg-require-effective-target untyped_assembly } */

int
main ()
{
  fputs ("");
  fputs_unlocked ("");
  return 0;
}

/* cp-out: warning: [^:]*: line 7, columns 2-7: identifier "fputs" not declared */
/* cp-out: warning: [^:]*: line 8, columns 2-16: identifier "fputs_unlocked" not declared */
