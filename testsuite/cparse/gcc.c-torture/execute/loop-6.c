main()
{
  char c;
  char d;
  int nbits;
  c = -1;
  for (nbits = 1 ; nbits < 100; nbits++) {
    d = (1 << nbits) - 1;
    if (d == c)
      break;
  }
  if (nbits == 100)
    abort();
  exit(0);
}

/* cp-out: warning: [^:]*: line 13, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 14, column 2: identifier "exit" not declared */
