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

/* cp-out: warning: [^:]*: line 5, columns 6-10: identifier "puts" not declared */
/* cp-out: warning: [^:]*: line 14, columns 2-8: identifier "printf" not declared */
