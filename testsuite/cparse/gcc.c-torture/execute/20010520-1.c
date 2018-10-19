static unsigned int expr_hash_table_size = 1;

int
main ()
{
  int del = 1;
  unsigned int i = 0;

  if (i < expr_hash_table_size && del)
    exit (0);
  abort ();
}

/* cp-out: warning: [^:]*: line 10, columns 4-8: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 11, columns 2-7: identifier "abort" not declared */
