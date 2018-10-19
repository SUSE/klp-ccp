/* This is from PR c/25892.  See Wpointer-sign.c for more details.  */

/* { dg-options "-Wall" } */

void foo(unsigned long* ulp); /* { dg-message "note: expected '\[^\n'\]*' but argument is of type '\[^\n'\]*'" "note: expected" } */


void bar(long* lp) {
  foo(lp); /* { dg-warning "differ in signedness" } */
}

/* cp-out: warning: [^:]*: line 9, columns 6-8: pointers to different integer types in assignment */
