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

/* cp-out: warning: [^:]*: line 28, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 30, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 32, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 34, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 35, columns 2-6: identifier "exit" not declared */
