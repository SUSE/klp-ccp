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

/* cp-out: warning: [^:]*: line 6, columns 2-3: identifier "f" not declared */
/* cp-out: warning: [^:]*: line 7, columns 2-3: identifier "f" not declared */
/* cp-out: warning: [^:]*: line 8, columns 2-6: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 15, columns 4-9: identifier "abort" not declared */
