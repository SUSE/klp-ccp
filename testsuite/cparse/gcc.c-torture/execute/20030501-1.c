int
main (int argc, char **argv)
{
  int size = 10;

  {
    int retframe_block()
      {
        return size + 5;
      }

  if (retframe_block() != 15)
      abort ();
  exit (0);

  }
}

/* cp-out: warning: [^:]*: line 13, column 6: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 14, column 2: identifier "exit" not declared */
