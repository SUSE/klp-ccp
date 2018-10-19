/* { dg-do compile { target mips*-*-* } } */

register unsigned int cp0count asm ("$c0r1");

int __attribute__ ((nomips16))
main (int argc, char *argv[])
{
  unsigned int d;

  d = cp0count + 3;
  printf ("%d\n", d);
}

/* cp-out: warning: [^:]*: line 11, columns 2-8: identifier "printf" not declared */
