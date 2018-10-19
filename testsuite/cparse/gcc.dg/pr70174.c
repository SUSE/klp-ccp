/* PR rtl-optimization/70174 */
/* { dg-do compile } */
/* { dg-options "-O2" } */

struct S { int f : 4; } a;
  
void
foo (void)
{ 
  a.f = foo;	/* { dg-warning "assignment to 'signed char:4' from 'void \\(\\*\\)\\(void\\)' makes integer from pointer without a cast" } */
}

/* cp-out: warning: [^:]*: line 10, columns 8-11: assignment from pointer to non-boolean bitfield type */
