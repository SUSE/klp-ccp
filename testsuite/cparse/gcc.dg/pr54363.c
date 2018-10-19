/* PR c/54363 */
/* { dg-do compile } */
/* { dg-options "-std=gnu99" } */

struct S { char **a; };

void
test (void)
{
  struct S b = { .a = (char **) { "a", "b" } }; /* { dg-warning "(initialization|excess elements)" } */
  struct S c = { .a = (char *[]) { "a", "b" } };
}

/* cp-out: warning: [^:]*: line 10, columns 34-37: invalid initialization of scalar type */
