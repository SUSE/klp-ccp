void func(void) __attribute__((aligned(256)));

void func(void) 
{
}

int main()
{
  if (__alignof__(func) != 256)
    abort ();
  return 0;
}

/* cp-out: warning: [^:]*: line 10, columns 4-9: identifier "abort" not declared */
