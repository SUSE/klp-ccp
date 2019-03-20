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

/* cp-out: warning: [^:]*: line 10, column 4: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 11, column 2: identifier "abort" not declared */
