int
f(long long x)
{
  x >>= 8;
  return x & 0xff;
}

main()
{
  if (f(0x0123456789ABCDEFLL) != 0xCD)
    abort();
  exit (0);
}

/* cp-out: warning: [^:]*: line 11, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 12, column 2: identifier "exit" not declared */
