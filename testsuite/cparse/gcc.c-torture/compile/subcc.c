int foo (a, c)
{
  int b;

  if (a + c >= 0)			/* b < 0 ==== a < 10? */
    return a | 0x80000000;
  return 0;
}

void bar (a)
     int a;
{
  if (foo (a, 10) & 0x80000000)
    printf ("y");
  else
    printf ("n");
}

int main ()
{
  bar (0);
  bar (1);
  bar (-1);
  bar (10);
  bar (-10);
  bar (11);
  bar (-11);
  bar (0x7fffffff);
  bar (-0x7fffffff);

  puts ("");
  return 0;
}

/* cp-out: warning: [^:]*: line 14, columns 4-10: identifier "printf" not declared */
/* cp-out: warning: [^:]*: line 16, columns 4-10: identifier "printf" not declared */
/* cp-out: warning: [^:]*: line 31, columns 2-6: identifier "puts" not declared */
