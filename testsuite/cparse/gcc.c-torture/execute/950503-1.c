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

/* cp-out: warning: [^:]*: line 12, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 13, columns 2-6: identifier "exit" not declared */
