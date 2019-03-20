unsigned int a[0x1000];
extern const unsigned long v;

main ()
{
  f (v);
  f (v);
  exit (0);
}

f (a)
     unsigned long a;
{
  if (a != 0xdeadbeefL)
    abort();
}

const unsigned long v = 0xdeadbeefL;

/* cp-out: warning: [^:]*: line 6, column 2: identifier "f" not declared */
/* cp-out: warning: [^:]*: line 7, column 2: identifier "f" not declared */
/* cp-out: warning: [^:]*: line 8, column 2: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 15, column 4: identifier "abort" not declared */
