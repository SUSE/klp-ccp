/* This is from PR c/25892.  The SC promised RMS that -Wpointer-sign
   would be off by default in GCC 4.1 to avoid inconvenient warnings
   while compiling GNU Emacs.  It should be enabled with -Wall and/or
   -pedantic, though.  Make sure it's off by default in this test (so
   use dg-options "" to avoid passing -pedantic-errors).  */

/* { dg-options "" } */

void foo(unsigned long* ulp);

void bar(long* lp) {
  foo(lp);
}

/* cp-out: warning: [^:]*: line 12, column 6: pointers to different integer types in assignment */
