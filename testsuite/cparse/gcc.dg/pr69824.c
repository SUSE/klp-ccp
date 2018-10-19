/* { dg-do compile } */
/* { dg-options "-w" } */
int bar() { return foo(); }
void baz(int c[foo()]) { return; }

/* cp-out: warning: [^:]*: line 3, columns 19-22: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 4, columns 15-18: identifier "foo" not declared */
