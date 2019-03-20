/* This is from PR c/25892.  See Wpointer-sign.c for more details.  */

/* { dg-options "-Wno-pointer-sign -pedantic" } */

void foo(unsigned long* ulp);

void bar(long* lp) {
  foo(lp);
}

/* cp-out: warning: [^:]*: line 8, column 6: pointers to different integer types in assignment */
