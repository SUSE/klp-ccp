unsigned long*
f(p)unsigned long*p;
{
  unsigned long a = (*p++) >> 24;
  return p + a;
}

main ()
{
  unsigned long x = 0x80000000UL;
  if (f(&x) != &x + 0x81)
    abort();
  exit(0);
}

/* cp-out: warning: [^:]*: line 12, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 13, column 2: identifier "exit" not declared */
