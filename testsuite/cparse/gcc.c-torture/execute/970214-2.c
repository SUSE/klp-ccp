#define m(L) (L'1' + (L))
main ()
{
  exit (m (0) != L'1');
}

/* cp-out: warning: [^:]*: line 4, column 2: identifier "exit" not declared */
