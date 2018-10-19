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

/* cp-out: warning: [^:]*: line 26, columns 6-12: identifier "printf" not declared */
/* cp-out: warning: [^:]*: line 28, columns 2-6: identifier "puts" not declared */
