f (unsigned char *a)
{
  int i, j;
  int x, y;

  j = a[1];
  i = a[0] - j;
  if (i < 0)
    {
      x = 1;
      y = -i;
    }
  else
    {
      x = 0;
      y =  i;
    }
  return x + y;
}


main ()
{
  unsigned char a[2];
  a[0] = 8;
  a[1] = 9;
  if (f (a) != 2)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 28, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 29, columns 2-6: identifier "exit" not declared */
