int b=1;
int foo()
{
  int a;
  int c;
  a=0xff;
  for (;b;b--)
  {
    c=1;
    asm(""::"r"(c));
    c=(signed char)a;
  }
  if (c!=-1)
    abort();
  return c;
}
int main()
{
  foo();
  return 0;
}

/* cp-out: warning: [^:]*: line 14, columns 4-9: identifier "abort" not declared */
