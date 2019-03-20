foo (p)
     int *p;
{
  return (*p & 255) == 0;
}

bar (a)
{
  return (a & 0xfff00000) == 0;
}

main ()
{
  printf ("%d%d\n", bar (-1), bar(0));
}

/* cp-out: warning: [^:]*: line 14, column 2: identifier "printf" not declared */
