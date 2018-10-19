double
foo (void)
{
  return 0.0;
}

void
do_sibcall (void)
{
  (void) foo ();
}

int
main (void)
{
   double x;

   for (x = 0; x < 20; x++)
      do_sibcall ();
   if (!(x >= 10))
      abort ();
   exit (0);
}

/* cp-out: warning: [^:]*: line 21, columns 6-11: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 22, columns 3-7: identifier "exit" not declared */
