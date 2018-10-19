struct complex
{
  float r;
  float i;
};

struct complex cmplx (float, float);

struct complex
f (float a, float b)
{
  struct complex c;
  c.r = a;
  c.i = b;
  return c;
}

main ()
{
  struct complex z = f (1.0, 0.0);

  if (z.r != 1.0 || z.i != 0.0)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 23, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 24, columns 2-6: identifier "exit" not declared */
