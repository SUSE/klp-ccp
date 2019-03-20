main()
{
  unsigned long L;
  double D;
  D = L = -3;
  printf("L=%lu, D=%g\n", L, D);
  return 0;
}

/* cp-out: warning: [^:]*: line 6, column 2: identifier "printf" not declared */
