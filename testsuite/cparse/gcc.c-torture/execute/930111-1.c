main()
{
  if (wwrite((long long) 0) != 123)
    abort();
  exit(0);
}

int
wwrite(long long i)
{
  switch(i)
    {
    case 3:
    case 10:
    case 23:
    case 28:
    case 47:
      return 0;
    default:
      return 123;
    }
}

/* cp-out: warning: [^:]*: line 3, column 6: identifier "wwrite" not declared */
/* cp-out: warning: [^:]*: line 4, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 5, column 2: identifier "exit" not declared */
