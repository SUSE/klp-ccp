f (int b, int c)
{
  if (b != 0 && b != 1 && c != 0)
    b = 0;
  return b;
}

main ()
{
  if (!f (1, 2))
    abort();
  exit(0);
}

/* cp-out: warning: [^:]*: line 11, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 12, column 2: identifier "exit" not declared */
