struct {
  int a:4;
  int :4;
  int b:4;
  int c:4;
} x = { 2,3,4 };

main ()
{
  if (x.a != 2)
    abort ();
  if (x.b != 3)
    abort ();
  if (x.c != 4)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 11, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 13, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 15, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 16, column 2: identifier "exit" not declared */
