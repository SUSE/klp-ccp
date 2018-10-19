f (c)
    unsigned char c;
{
  if (c != 0xFF)
    abort ();
}

main ()
{
  f (-1);
  exit (0);
}

/* cp-out: warning: [^:]*: line 5, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 11, columns 2-6: identifier "exit" not declared */
