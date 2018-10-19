struct foo
{
  unsigned half:16;
  unsigned long whole:32 __attribute__ ((packed));
};

f (struct foo *q)
{
  if (q->half != 0x1234)
    abort ();
  if (q->whole != 0x56789abcL)
    abort ();
}

main ()
{
  struct foo bar;

  bar.half = 0x1234;
  bar.whole = 0x56789abcL;
  f (&bar);
  exit (0);
}

/* cp-out: warning: [^:]*: line 10, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 12, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 22, columns 2-6: identifier "exit" not declared */
