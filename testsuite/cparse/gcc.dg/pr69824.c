/* { dg-do compile } */
/* { dg-options "-w" } */
int bar() { return foo(); }
void baz(int c[foo()]) { return; }

/* cp-out: warning: [^:]*: line 3, column 19: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 4, column 15: identifier "foo" not declared */
