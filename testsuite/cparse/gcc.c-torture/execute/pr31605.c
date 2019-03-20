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

/* cp-out: warning: [^:]*: line 5, column 4: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 12, column 2: identifier "abort" not declared */
