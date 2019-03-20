short x1 = 17;

struct
{
  short i __attribute__ ((packed));
} t;

f ()
{
  t.i = x1;
  if (t.i != 17)
    abort ();
}

main ()
{
  f ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 12, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 18, column 2: identifier "exit" not declared */
