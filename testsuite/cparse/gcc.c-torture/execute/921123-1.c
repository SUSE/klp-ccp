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

/* cp-out: warning: [^:]*: line 11, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 12, column 2: identifier "exit" not declared */
