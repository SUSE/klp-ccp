int f (int a, int b) { }

main ()
{
  unsigned long addr1;
  unsigned long addr2;

  addr1 = (unsigned long) &f;
  addr1 += 5;
  addr2 = 5 + (unsigned long) &f;

  if (addr1 != addr2)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 13, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 14, column 2: identifier "exit" not declared */
