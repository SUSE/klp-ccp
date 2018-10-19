/* { dg-do compile } */
/* { dg-options "-O2 -std=c99" } */

void
foo (void)
{
  char *e = alloca (100); /* { dg-warning "implicit declaration|initialization of 'char \\*' from 'int' makes" } */
}

/* cp-out: warning: [^:]*: line 7, columns 12-18: identifier "alloca" not declared */
/* cp-out: warning: [^:]*: line 7, columns 12-18: assignment to pointer from integer which is not NULL */
