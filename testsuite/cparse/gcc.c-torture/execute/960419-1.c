static int i;

void
check(x)
     int x;
{
  if (!x)
    abort();
}

main()
{
  int *p = &i;

  check(p != (void *)0);
  exit (0);
}

/* cp-out: warning: [^:]*: line 8, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 16, columns 2-6: identifier "exit" not declared */
