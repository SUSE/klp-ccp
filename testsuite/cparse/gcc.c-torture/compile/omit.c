omit (a, b)
    char a;
    char *b;
{
  char x;
  int i;
  x = *b;
  b[1] = x;
  foo ((int)x);
  return x + 1;
}

/* cp-out: warning: [^:]*: line 9, columns 2-5: identifier "foo" not declared */
