main ()
{
  struct { long status; } h;

  h.status = 0;
  if (((h.status & 128) == 1) && ((h.status & 32) == 0))
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 7, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 8, column 2: identifier "exit" not declared */
