/* PR c/81233 */
/* { dg-do compile } */
/* { dg-options "-Wc++-compat -Wpedantic" } */
/* Test we're printing the types, like the good compiler we are.  */

enum E1 { A } e;
enum E2 { B };
extern void foo_E (enum E1); /* { dg-message "expected 'enum E1' but argument is of type 'int'" } */
extern void foo (char *); /* { dg-message "expected 'char \\*' but argument is of type 'int \\*'" } */
extern void foo2 (int *); /* { dg-message "expected 'int \\*' but argument is of type 'int'" } */
extern void foo3 (int); /* { dg-message "expected 'int' but argument is of type 'int \\*'" } */
extern void foo4 (int *); /* { dg-message "expected 'int \\*' but argument is of type 'unsigned int \\*'" } */

char *
fn0 (int *p, char *q)
{
  p = q; /* { dg-warning "assignment to 'int \\*' from incompatible pointer type 'char \\*'" } */
  int *r = q; /* { dg-warning "initialization of 'int \\*' from incompatible pointer type 'char \\*'" } */
  foo (r); /* { dg-warning "passing argument 1 of 'foo' from incompatible pointer type" } */
  return p; /* { dg-warning "returning 'int \\*' from a function with incompatible return type 'char \\*'" } */
}

int *
fn1 (int *p)
{
  p = 1; /* { dg-warning "assignment to 'int \\*' from 'int' makes pointer from integer without a cast" } */
  int *q = 1; /* { dg-warning "initialization of 'int \\*' from 'int' makes pointer from integer without a cast" } */
  foo2 (1); /* { dg-warning "passing argument 1 of 'foo2' makes pointer from integer without a cast" } */
  return 1; /* { dg-warning "returning 'int' from a function with return type 'int \\*' makes pointer from integer without a cast" } */
}

int
fn2 (int i, int *p)
{
  i = p; /* { dg-warning "assignment to 'int' from 'int \\*' makes integer from pointer without a cast" } */
  int j = p; /* { dg-warning "initialization of 'int' from 'int \\*' makes integer from pointer without a cast" } */
  foo3 (p); /* { dg-warning "passing argument 1 of 'foo3' makes integer from pointer without a cast" } */
  return p; /* { dg-warning "returning 'int \\*' from a function with return type 'int' makes integer from pointer without a cast" } */
}

int *
fn3 (int *p, unsigned int *u)
{
  p = u; /* { dg-warning "pointer targets in assignment from 'unsigned int \\*' to 'int \\*' differ in signedness" } */
  int *q = u; /* { dg-warning "pointer targets in initialization of 'int \\*' from 'unsigned int \\*' differ in signedness" } */
  foo4 (u); /* { dg-warning "pointer targets in passing argument 1 of 'foo4' differ in signedness" } */
  return u; /* { dg-warning "pointer targets in returning 'unsigned int \\*' from a function with return type 'int \\*' differ in signedness" } */
}

enum E1
fn4 (void)
{
  foo_E (B); /* { dg-warning "enum conversion when passing argument" } */
  e = 0; /* { dg-warning "enum conversion from 'int' to 'enum E1' in assignment is invalid" } */
  enum E1 f = 0; /* { dg-warning "enum conversion from 'int' to 'enum E1' in initialization is invalid" } */
  return 0; /* { dg-warning "enum conversion from 'int' to 'enum E1' in return is invalid" } */
}

/* cp-out: warning: [^:]*: line 17, column 6: assigning pointers to integer types of different width */
/* cp-out: warning: [^:]*: line 18, column 11: assigning pointers to integer types of different width */
/* cp-out: warning: [^:]*: line 19, column 7: assigning pointers to integer types of different width */
/* cp-out: warning: [^:]*: line 20, column 9: assigning pointers to integer types of different width */
/* cp-out: warning: [^:]*: line 26, column 6: assignment to pointer from integer which is not NULL */
/* cp-out: warning: [^:]*: line 27, column 11: assignment to pointer from integer which is not NULL */
/* cp-out: warning: [^:]*: line 28, column 8: assignment to pointer from integer which is not NULL */
/* cp-out: warning: [^:]*: line 29, column 9: assignment to pointer from integer which is not NULL */
/* cp-out: warning: [^:]*: line 35, column 6: assignment to integer from pointer */
/* cp-out: warning: [^:]*: line 36, column 10: assignment to integer from pointer */
/* cp-out: warning: [^:]*: line 37, column 8: assignment to integer from pointer */
/* cp-out: warning: [^:]*: line 38, column 9: assignment to integer from pointer */
/* cp-out: warning: [^:]*: line 44, column 6: pointers to different integer types in assignment */
/* cp-out: warning: [^:]*: line 45, column 11: pointers to different integer types in assignment */
/* cp-out: warning: [^:]*: line 46, column 8: pointers to different integer types in assignment */
/* cp-out: warning: [^:]*: line 47, column 9: pointers to different integer types in assignment */
