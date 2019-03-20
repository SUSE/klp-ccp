#define L 1
main ()
{
  exit (L'1' != L'1');
}

/* cp-out: warning: [^:]*: line 4, column 2: identifier "exit" not declared */
