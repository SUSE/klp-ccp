main ()
{
  unsigned char z = 0;

  do ;
  while (--z > 0);
  exit (0);
}

/* cp-out: warning: [^:]*: line 7, column 2: identifier "exit" not declared */
