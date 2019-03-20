barfoo (a)
{
  return (a << 16) & ~0xffff;
}

foobar (a)
{
  return ((unsigned short) a) << 15;}

foo (a)
{
  return (a & 0x121) << 31;
}

bar (a)
{
  return (a & ~0xffff) << 16;
}

main ()
{
  int a;

  for (a = 1;  a; a += a)
    {
      printf ("%d", (foo (a)));
    }
  puts ("");
}

/* cp-out: warning: [^:]*: line 26, column 6: identifier "printf" not declared */
/* cp-out: warning: [^:]*: line 28, column 2: identifier "puts" not declared */
