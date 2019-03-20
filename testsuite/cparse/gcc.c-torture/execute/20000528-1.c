/* Copyright (C) 2000  Free Software Foundation  */
/* Contributed by Alexandre Oliva <aoliva@cygnus.com> */

unsigned long l = (unsigned long)-2;
unsigned short s;

int main () {
  long t = l;
  s = t;
  if (s != (unsigned short)-2)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 11, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 12, column 2: identifier "exit" not declared */
