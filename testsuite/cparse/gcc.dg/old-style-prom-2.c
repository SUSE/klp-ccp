/* Test for prototype followed by old-style definition, as in
   dremf-type-compat-2.c but with a non-built-in function.  */
/* { dg-do compile } */
/* { dg-options "-pedantic" } */

float f (float, float); /* { dg-warning "prototype declaration" } */

float
f (x, y)
     float x; /* { dg-warning "promoted argument 'x' doesn't match prototype" } */
     float y; /* { dg-warning "promoted argument 'y' doesn't match prototype" } */
{
  return x + y;
}

/* cp-out: warning: [^:]*: line 9, columns 0-1: incompatible redeclaration */
