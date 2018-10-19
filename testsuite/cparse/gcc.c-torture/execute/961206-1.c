int
sub1 (unsigned long long i)
{
  if (i < 0x80000000)
    return 1;
  else
    return 0;
}

int
sub2 (unsigned long long i)
{
  if (i <= 0x7FFFFFFF)
    return 1;
  else
    return 0;
}

int
sub3 (unsigned long long i)
{
  if (i >= 0x80000000)
    return 0;
  else
    return 1;
}

int
sub4 (unsigned long long i)
{
  if (i > 0x7FFFFFFF)
    return 0;
  else
    return 1;
}

main()
{
  if (sub1 (0x80000000ULL))
    abort ();

  if (sub2 (0x80000000ULL))
    abort ();

  if (sub3 (0x80000000ULL))
    abort ();

  if (sub4 (0x80000000ULL))
    abort ();

  exit (0);
}

/* cp-out: warning: [^:]*: line 40, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 43, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 46, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 49, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 51, columns 2-6: identifier "exit" not declared */
