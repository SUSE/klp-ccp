foo (a)
{
  while ((a -= 1) != -1)
    bar (270000);
  putchar ('\n');
}

main ()
{
  foo (5);
}

/* cp-out: warning: [^:]*: line 4, columns 4-7: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 5, columns 2-9: identifier "putchar" not declared */
