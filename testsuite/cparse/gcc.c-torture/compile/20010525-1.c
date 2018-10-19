/* { dg-require-effective-target untyped_assembly } */
static int kind_varread(char *str)
{
  if (0 == memcmp("%_#",               str, 3))  return 2;
  /* NOTREACHED */
}

/* cp-out: warning: [^:]*: line 4, columns 11-17: identifier "memcmp" not declared */
