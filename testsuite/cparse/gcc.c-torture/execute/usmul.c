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

/* cp-out: warning: [^:]*: line 15, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 17, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 19, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 21, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 24, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 26, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 28, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 30, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 32, column 2: identifier "exit" not declared */
