int a;
int
main ()
{
  int b = 0;
  while (a < 0 || b)
    {
      b = 0;
      for (; b < 9; b++)
	;
    }
  exit (0);
}

/* cp-out: warning: [^:]*: line 12, columns 2-6: identifier "exit" not declared */
