/* Test diagnostic for array defaulting to one element.  */
/* Origin: Joseph Myers <joseph@codesourcery.com> */
/* { dg-do compile } */
/* { dg-options "" } */

int a[]; /* { dg-warning "array 'a' assumed to have one element" } */

/* cp-out: warning: [^:]*: line 6, columns 4-7: init declarator has incomplete type */
