/* { dg-do compile { target mips*-*-* } } */

register unsigned int c3r1 asm ("$c3r1");

extern unsigned int b, c;

void __attribute__ ((nomips16))
foo ()
{
  unsigned int a, d;

  c3r1 = a;
  b = c3r1;

  c3r1 = c;
  d = c3r1;
  printf ("%d\n", d);
}

/* cp-out: warning: [^:]*: line 17, columns 2-8: identifier "printf" not declared */
