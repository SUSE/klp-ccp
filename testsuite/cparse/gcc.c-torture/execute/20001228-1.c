int foo1(void)
{
  union {
    char a[sizeof (unsigned)];
    unsigned b;
  } u;
  
  u.b = 0x01;
  return u.a[0];
}

int foo2(void)
{
  volatile union {
    char a[sizeof (unsigned)];
    unsigned b;
  } u;
  
  u.b = 0x01;
  return u.a[0];
}

int main(void)
{
  if (foo1() != foo2())
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 26, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 27, column 2: identifier "exit" not declared */
