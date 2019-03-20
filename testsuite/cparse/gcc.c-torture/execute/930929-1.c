sub1 (i)
     int i;
{
  return i - (5 - i);
}

sub2 (i)
     int i;
{
  return i + (5 + i);
}

sub3 (i)
     int i;
{
  return i - (5 + i);
}

sub4 (i)
     int i;
{
  return i + (5 - i);
}

main()
{
  if (sub1 (20) != 35)
    abort ();
  if (sub2 (20) != 45)
    abort ();
  if (sub3 (20) != -5)
    abort ();
  if (sub4 (20) != 5)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 28, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 30, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 32, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 34, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 35, column 2: identifier "exit" not declared */
