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

/* cp-out: warning: [^:]*: line 5, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 10, columns 2-6: identifier "exit" not declared */
