/* PR28322: ignore unknown -Wno-* if no warning is emitted.  */
/* { dg-do compile } */
/* { dg-options " -Wno-foobar -Wno-div-by-zero" } */

void foo(void)
{
  int i =  1/0;
}

/* cp-out: warning: [^:]*: line 7, columns 11-14: division by zero */
