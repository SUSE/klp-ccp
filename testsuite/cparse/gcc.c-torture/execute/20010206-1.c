int foo (void)
{
  int i;
#line 1 "20010206-1.c"
  if (0) i = 1; else i
#line 1 "20010206-1.c"
    = 26;
  return i;
}

int main ()
{
  if (foo () != 26)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 14, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 15, columns 2-6: identifier "exit" not declared */
