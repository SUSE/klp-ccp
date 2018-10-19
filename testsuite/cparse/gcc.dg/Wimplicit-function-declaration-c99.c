/* { dg-do compile } */
/* { dg-options "-std=c99" } */

void f(void) 
{ 
  puts("Hello"); /* { dg-warning "implicit declaration of function" } */
}

/* cp-out: warning: [^:]*: line 6, columns 2-6: identifier "puts" not declared */
