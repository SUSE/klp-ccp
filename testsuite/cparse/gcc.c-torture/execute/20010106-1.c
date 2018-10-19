/* Copyright 2001 Free Software Foundation
   Contributed by Alexandre Oliva <aoliva@redhat.com> */

int f(int i) {
  switch (i)
  {
    case -2:
      return 33;
    case -1:
      return 0;
    case 0:
      return 7;
    case 1:
      return 4;
    case 2:
      return 3;
    case 3:
      return 15;
    case 4:
     return 9;
    default:
      abort ();
  }
}

int main() {
  if (f(-1))
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 22, columns 6-11: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 28, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 29, columns 2-6: identifier "exit" not declared */
