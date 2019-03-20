main()
{
  if ((double) 18446744073709551615ULL < 1.84467440737095e+19 ||
      (double) 18446744073709551615ULL > 1.84467440737096e+19)
    abort();

  if (16777217L != (float)16777217e0)
    abort();

  exit(0);
}

/* cp-out: warning: [^:]*: line 5, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 8, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 10, column 2: identifier "exit" not declared */
