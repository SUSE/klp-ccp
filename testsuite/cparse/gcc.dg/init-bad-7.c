/* PR c/37724 */
/* { dg-do compile } */
/* { dg-options "-std=gnu99 -pedantic" } */

struct f
{
  int *a;
};

char b[10];
struct f g = {b}; /* { dg-warning "initialization of 'int \\*' from incompatible pointer type|near initialization for" } */

/* cp-out: warning: [^:]*: line 11, column 14: assigning pointers to integer types of different width */
