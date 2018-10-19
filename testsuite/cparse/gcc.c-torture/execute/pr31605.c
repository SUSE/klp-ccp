void put_field (unsigned int start, unsigned int len)
{
  int cur_bitshift = ((start + len) % 8) - 8;
  if (cur_bitshift > -8)
    exit (0);
}

int
main ()
{
  put_field (0, 1);
  abort ();
}

/* cp-out: warning: [^:]*: line 5, columns 4-8: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 12, columns 2-7: identifier "abort" not declared */
