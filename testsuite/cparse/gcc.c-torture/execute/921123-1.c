f(short *p)
{
  short x = *p;
  return (--x < 0);
}

main()
{
  short x = -10;
  if (!f(&x))
    abort();
  exit(0);
}

/* cp-out: warning: [^:]*: line 11, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 12, columns 2-6: identifier "exit" not declared */
