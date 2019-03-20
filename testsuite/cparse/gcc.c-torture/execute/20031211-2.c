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

/* cp-out: warning: [^:]*: line 11, column 2: identifier "foo" not declared */
/* cp-out: warning: [^:]*: line 12, column 2: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 18, column 4: identifier "abort" not declared */
