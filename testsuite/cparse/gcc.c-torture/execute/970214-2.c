#define m(L) (L'1' + (L))
main ()
{
  exit (m (0) != L'1');
}

/* cp-out: warning: [^:]*: line 4, columns 2-6: identifier "exit" not declared */
