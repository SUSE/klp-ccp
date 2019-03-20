static void
compare (long long foo)
{
  if (foo < 4294967297LL)
    abort();
}
int main(void)
{
  compare (8589934591LL);
  exit (0);
}

/* cp-out: warning: [^:]*: line 5, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 10, column 2: identifier "exit" not declared */
