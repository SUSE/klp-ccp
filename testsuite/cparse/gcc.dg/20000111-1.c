/* Copyright (C) 2000  Free Software Foundation.

   by Alexandre Oliva  <oliva@lsd.ic.unicamp.br>  */

/* { dg-do compile } */

__inline int
foo (int **q) {
  return *q && **q;
}

void
bar () {
  int **p;
  if (foo (p))
    do_something ();
}

/* cp-out: warning: [^:]*: line 16, column 4: identifier "do_something" not declared */
