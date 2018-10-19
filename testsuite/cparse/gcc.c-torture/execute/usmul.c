/* { dg-require-effective-target int32plus } */
int __attribute__ ((noinline)) foo (short x, unsigned short y)
{
  return x * y;
}

int __attribute__ ((noinline)) bar (unsigned short x, short y)
{
  return x * y;
}

int main ()
{
  if (foo (-2, 0xffff) != -131070)
    abort ();
  if (foo (2, 0xffff) != 131070)
    abort ();
  if (foo (-32768, 0x8000) != -1073741824)
    abort ();
  if (foo (32767, 0x8000) != 1073709056)
    abort ();

  if (bar (0xffff, -2) != -131070)
    abort ();
  if (bar (0xffff, 2) != 131070)
    abort ();
  if (bar (0x8000, -32768) != -1073741824)
    abort ();
  if (bar (0x8000, 32767) != 1073709056)
    abort ();

  exit (0);
}

/* cp-out: warning: [^:]*: line 15, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 17, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 19, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 21, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 24, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 26, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 28, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 30, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 32, columns 2-6: identifier "exit" not declared */
