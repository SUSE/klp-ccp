main ()
{
  int tmp;
  unsigned long long utmp1, utmp2;

  tmp = 16;

  utmp1 = (~((unsigned long long) 0)) >> tmp;
  utmp2 = (~((unsigned long long) 0)) >> 16;

  if (utmp1 != utmp2)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 12, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 13, column 2: identifier "exit" not declared */
