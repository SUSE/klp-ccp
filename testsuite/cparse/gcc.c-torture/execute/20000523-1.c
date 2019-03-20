int
main (void)
{
  long long   x;
  int         n;

  if (sizeof (long long) < 8)
    exit (0);
  
  n = 9;
  x = (((long long) n) << 55) / 0xff; 

  if (x == 0)
    abort ();

  x = (((long long) 9) << 55) / 0xff;

  if (x == 0)
    abort ();

  exit (0);
}

/* cp-out: warning: [^:]*: line 8, column 4: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 14, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 19, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 21, column 2: identifier "exit" not declared */
