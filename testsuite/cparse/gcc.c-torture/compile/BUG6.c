main()
{
  unsigned long L;
  double D;
  D = L = -3;
  printf("L=%lu, D=%g\n", L, D);
  return 0;
}

/* cp-out: warning: [^:]*: line 6, columns 2-8: identifier "printf" not declared */
