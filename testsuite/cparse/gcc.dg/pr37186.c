/* PR 37186 */
/* { dg-do compile } */
/* { dg-options "-Wall -Werror -Wno-error=pointer-sign" } */

int foo(signed char *); /* { dg-message "note: expected 'signed char \\*' but argument is of type 'unsigned char \\*'" } */
int bar(unsigned char *p)
{
  return foo(p); /* { dg-warning "pointer targets in passing argument 1 of 'foo' differ in signedness" } */
}

/* cp-out: warning: [^:]*: line 8, columns 13-14: pointers to different integer types in assignment */
