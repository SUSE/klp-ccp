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

/* cp-out: warning: [^:]*: line 4, column 4: identifier "bar" not declared */
/* cp-out: warning: [^:]*: line 5, column 2: identifier "putchar" not declared */
