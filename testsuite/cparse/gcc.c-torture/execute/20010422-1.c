unsigned int foo(unsigned int x)
{
  if (x < 5)
    x = 4;
  else
    x = 8;
  return x;
}

int main(void)
{
  if (foo (8) != 8)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 13, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 14, columns 2-6: identifier "exit" not declared */
