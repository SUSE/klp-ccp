div1 (signed char x)
{
  return x / -1;
}

div2 (signed short x)
{
  return x / -1;
}

div3 (signed char x, signed char y)
{
  return x / y;
}

div4 (signed short x, signed short y)
{
  return x / y;
}

mod1 (signed char x)
{
  return x % -1;
}

mod2 (signed short x)
{
  return x % -1;
}

mod3 (signed char x, signed char y)
{
  return x % y;
}

mod4 (signed short x, signed short y)
{
  return x % y;
}

signed long
mod5 (signed long x, signed long y)
{
  return x % y;
}
     
unsigned long
mod6 (unsigned long x, unsigned long y)
{
  return x % y;
}
     
main ()
{
  if (div1 (-(1 << 7)) != 1 << 7)
    abort ();
  if (div2 (-(1 << 15)) != 1 << 15)
    abort ();
  if (div3 (-(1 << 7), -1) != 1 << 7)
    abort ();
  if (div4 (-(1 << 15), -1) != 1 << 15)
    abort ();
  if (mod1 (-(1 << 7)) != 0)
    abort ();
  if (mod2 (-(1 << 15)) != 0)
    abort ();
  if (mod3 (-(1 << 7), -1) != 0)
    abort ();
  if (mod4 (-(1 << 15), -1) != 0)
    abort ();
  if (mod5 (0x50000000, 2) != 0)
    abort ();
  if (mod6 (0x50000000, 2) != 0)
    abort ();
  
  exit (0);
}

/* cp-out: warning: [^:]*: line 56, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 58, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 60, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 62, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 64, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 66, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 68, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 70, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 72, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 74, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 76, column 2: identifier "exit" not declared */
