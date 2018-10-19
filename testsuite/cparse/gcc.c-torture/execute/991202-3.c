
unsigned int f (unsigned int a)
{
  return a * 65536 / 8;
}

unsigned int g (unsigned int a)
{
  return a * 65536;
}

unsigned int h (unsigned int a)
{
  return a / 8;
}

int main ()
{
  if (f (65536) != h (g (65536)))
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 20, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 21, columns 2-6: identifier "exit" not declared */
