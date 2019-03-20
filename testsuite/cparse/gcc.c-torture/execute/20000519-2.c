long x = -1L;

int main()
{
  long b = (x != -1L);

  if (b)
    abort();

  exit(0);
}

/* cp-out: warning: [^:]*: line 8, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 10, column 2: identifier "exit" not declared */
