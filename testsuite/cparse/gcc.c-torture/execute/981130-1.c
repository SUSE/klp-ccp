/* { dg-xfail-if "alias analysis conflicts with instruction scheduling" { m32r-*-* } { "-O2" "-O1" "-O0" "-Os"} { "" } } */
struct s { int a; int b;};
struct s s1;
struct s s2 = { 1, 2, };

void
check (a, b)
     int a;
     int b;
{
  if (a == b)
    exit (0);
  else
    abort ();
}

int
main ()
{
  int * p;
  int x;
  
  s1.a = 9;
  p    = & s1.a;
  s1   = s2;
  x    = * p;
  
  check (x, 1);
}

/* cp-out: warning: [^:]*: line 12, columns 4-8: identifier "exit" not declared */
/* cp-out: warning: [^:]*: line 14, columns 4-9: identifier "abort" not declared */
