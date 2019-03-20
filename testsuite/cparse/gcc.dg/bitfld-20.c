/* Test for bit-field widths not integer constant expressions but
   folding to integer constants: PR 42439.  */
/* { dg-do compile } */
/* { dg-options "-O2 -pedantic" } */

void
f (void)
{
  const int m = 1;
  ((void)(sizeof(struct { int i:!!m; }))); /* { dg-warning "not an integer constant expression" } */
}

/* cp-out: error: [^:]*: line 10, columns 32-35: bit-field width is not a constant expression */
