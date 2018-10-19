char foo(char bar[])
{
  return bar[1];
}
extern char foo(char *);
int main(void)
{
  if (foo("xy") != 'y')
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 9, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 10, columns 2-6: identifier "exit" not declared */
