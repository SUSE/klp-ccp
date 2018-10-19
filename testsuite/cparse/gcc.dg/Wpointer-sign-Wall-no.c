/* This is from PR c/25892.  See Wpointer-sign.c for more details.  */

/* { dg-options "-Wno-pointer-sign -Wall" } */

void foo(unsigned long* ulp);

void bar(long* lp) {
  foo(lp);
}

/* cp-out: warning: [^:]*: line 8, columns 6-8: pointers to different integer types in assignment */

