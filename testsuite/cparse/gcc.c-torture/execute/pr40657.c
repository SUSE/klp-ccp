/* Verify that that Thumb-1 epilogue size optimization does not clobber the
   return value.  */

long long v = 0x123456789abc;

__attribute__((noinline)) void bar (int *x)
{
  asm volatile ("" : "=m" (x) ::);
}

__attribute__((noinline)) long long foo()
{
  int x;
  bar(&x);
  return v;
}

int main ()
{
  if (foo () != v)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 21, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 22, columns 2-6: identifier "exit" not declared */
