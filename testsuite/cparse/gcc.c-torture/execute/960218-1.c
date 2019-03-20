int glob;

g (x)
{
  glob = x;
  return 0;
}

f (x)
{
  int a = ~x;
  while (a)
    a = g (a);
}

main ()
{
  f (3);
  if (glob != -4)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 20, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 21, column 2: identifier "exit" not declared */
