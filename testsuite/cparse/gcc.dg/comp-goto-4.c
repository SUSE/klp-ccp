/* PR middle-end/79537 */
/* { dg-do compile } */
/* { dg-options "" } */
/* { dg-require-effective-target indirect_jumps } */
/* { dg-require-effective-target label_values } */

void
f (void)
{
L:
  *&&L;
}

void
f2 (void)
{
   void *p;
L:
   p = &&L;
   *p; /* { dg-warning "dereferencing 'void \\*' pointer" } */
}

/* cp-out: warning: [^:]*: line 11, columns 3-5: dereferencing pointer to incomplete type */
/* cp-out: warning: [^:]*: line 20, columns 4-5: dereferencing pointer to incomplete type */
