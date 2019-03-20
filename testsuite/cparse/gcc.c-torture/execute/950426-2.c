main()
{
  long int i = -2147483647L - 1L; /* 0x80000000 */
  char ca = 1;

  if (i >> ca != -1073741824L)
    abort ();

  if (i >> i / -2000000000L != -1073741824L)
    abort ();

  exit (0);
}

/* cp-out: warning: [^:]*: line 7, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 10, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 12, column 2: identifier "exit" not declared */
