foo (a)
{
  do
    {
      puts ("a");
      a -= 1;
    }
  while (a != 0);
}

main ()
{
  int p[100];
  printf ("%d\n", foo (3));
}

/* cp-out: warning: [^:]*: line 5, column 6: identifier "puts" not declared */
/* cp-out: warning: [^:]*: line 14, column 2: identifier "printf" not declared */
