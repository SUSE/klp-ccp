unsigned long long
main ()
{
  return (unsigned long long) 7816234 << 671111;
}

/* cp-out: warning: [^:]*: line 4, columns 9-47: shift distance exceeds integer width */
