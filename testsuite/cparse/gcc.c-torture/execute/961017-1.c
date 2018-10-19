main ()
{
  unsigned char z = 0;

  do ;
  while (--z > 0);
  exit (0);
}

/* cp-out: warning: [^:]*: line 7, columns 2-6: identifier "exit" not declared */
