/* PR c/23075 */
/* { dg-do compile } */
/* { dg-options "-O2 -Wreturn-type" } */

int
foo (void)
{
  return;	/* { dg-warning "with no value" } */
}		/* { dg-bogus "control reaches end" } */

int
bar (void)
{
}		/* { dg-warning "control reaches end" } */

/* cp-out: warning: [^:]*: line 8, columns 2-8: return without value in function returning non-void */
