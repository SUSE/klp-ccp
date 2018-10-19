int main ()
{
  long double x;

  x = 0x1.0p-500L;
  x *= 0x1.0p-522L;
  if (x != 0x1.0p-1022L)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 8, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 9, columns 2-6: identifier "exit" not declared */
