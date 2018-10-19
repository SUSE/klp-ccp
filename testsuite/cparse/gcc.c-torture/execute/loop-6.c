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

/* cp-out: warning: [^:]*: line 13, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 14, columns 2-6: identifier "exit" not declared */
