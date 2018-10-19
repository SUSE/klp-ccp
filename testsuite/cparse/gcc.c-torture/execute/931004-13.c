struct tiny
{
  char c;
  char d;
  char e;
  char f;
};

f (int n, struct tiny x, struct tiny y, struct tiny z, long l)
{
  if (x.c != 10)
    abort();
  if (x.d != 20)
    abort();
  if (x.e != 30)
    abort();
  if (x.f != 40)
    abort();

  if (y.c != 11)
    abort();
  if (y.d != 21)
    abort();
  if (y.e != 31)
    abort();
  if (y.f != 41)
    abort();

  if (z.c != 12)
    abort();
  if (z.d != 22)
    abort();
  if (z.e != 32)
    abort();
  if (z.f != 42)
    abort();

  if (l != 123)
    abort ();
}

main ()
{
  struct tiny x[3];
  x[0].c = 10;
  x[1].c = 11;
  x[2].c = 12;
  x[0].d = 20;
  x[1].d = 21;
  x[2].d = 22;
  x[0].e = 30;
  x[1].e = 31;
  x[2].e = 32;
  x[0].f = 40;
  x[1].f = 41;
  x[2].f = 42;
  f (3, x[0], x[1], x[2], (long) 123);
  exit(0);
}

/* cp-out: warning: [^:]*: line 12, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 14, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 16, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 18, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 21, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 23, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 25, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 27, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 30, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 32, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 34, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 36, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 39, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 58, columns 2-6: identifier "exit" not declared */
