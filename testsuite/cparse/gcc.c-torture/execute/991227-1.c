char* doit(int flag)
{
  return 1 + (flag ? "\0wrong\n" : "\0right\n");
}
int main()
{
  char *result = doit(0);
  if (*result == 'r' && result[1] == 'i')
    exit(0);
  abort();
}

/* cp-out: warning: [^:]*: line 9, column 4: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 10, column 2: identifier "abort" not declared */
