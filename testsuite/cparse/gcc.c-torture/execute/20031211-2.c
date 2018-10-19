struct a
{
  unsigned int bitfield : 3;
};

int main()
{
  struct a a;

  a.bitfield = 131;
  foo (a.bitfield);
  exit (0);
}

foo(unsigned int z)
{
  if (z != 3)
    abort ();
}

/* cp-out: warning: [^:]*: line 11, columns 2-5: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 12, columns 2-6: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 18, columns 4-9: identifier "abort" not declared */
