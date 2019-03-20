double
u2d (unsigned int u)
{
  return u;
}

double
i2d (signed int i)
{
  return i;
}

unsigned int
d2u (double d)
{
  return d;
}

signed int
d2i (double d)
{
  return d;
}

main ()
{
  printf ("%lf, %lf, %lf\n", u2d (~0), u2d (1 << 31), u2d (1));
  printf ("%lf, %lf, %lf\n", i2d (~0), i2d (1 << 31), i2d (1));

  printf ("%u, %u, %u\n", d2u (u2d (~0)), d2u (u2d (1 << 31)), d2u (u2d (1)));
  printf ("%d, %d, %d\n", d2i (i2d (~0)), d2i (i2d (1 << 31)), d2i (i2d (1)));
}

/* cp-out: warning: [^:]*: line 27, column 2: identifier "printf" not declared */
/* cp-out: warning: [^:]*: line 28, column 2: identifier "printf" not declared */
/* cp-out: warning: [^:]*: line 30, column 2: identifier "printf" not declared */
/* cp-out: warning: [^:]*: line 31, column 2: identifier "printf" not declared */
/* cp-out: warning: [^:]*: line 27, columns 44-51: integer overflow in shift operation */
/* cp-out: warning: [^:]*: line 28, columns 44-51: integer overflow in shift operation */
/* cp-out: warning: [^:]*: line 30, columns 52-59: integer overflow in shift operation */
/* cp-out: warning: [^:]*: line 31, columns 52-59: integer overflow in shift operation */
