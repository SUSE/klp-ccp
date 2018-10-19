int
f (b)
{
  return (b >> 1) > 0;
}

main ()
{
  if (!f (9))
    abort ();
  if (f (-9))
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 10, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 12, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 13, columns 2-6: identifier "exit" not declared */
