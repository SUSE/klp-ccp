int bar = 0;

f (p)
     int *p;
{
  int foo = 2;

  while (foo > bar)
    {
      foo -=  bar;
      *p++ = foo;
      bar = 1;
    }
}

main ()
{
  int tab[2];
  tab[0] = tab[1] = 0;
  f (tab);
  if (tab[0] != 2 || tab[1] != 1)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 22, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 23, column 2: identifier "exit" not declared */
