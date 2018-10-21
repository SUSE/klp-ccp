/* PR c/68024 */
/* { dg-do compile } */

void f (int, ...);
void f (a) int a; {} /* { dg-warning "defined as variadic function without prototype" } */

/* cp-out: warning: [^:]*: line 5, columns 5-6: incompatible redeclaration */
