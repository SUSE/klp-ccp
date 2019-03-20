int var = 0;

g ()
{
  var = 1;
}

f ()
{
  int f2 = 0;

  if (f2 == 0)
    ;

  g ();
}

main ()
{
  f ();
  if (var != 1)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 22, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 23, column 2: identifier "exit" not declared */
