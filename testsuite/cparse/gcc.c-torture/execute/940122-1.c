char *a = 0;
char *b = 0;

g (x)
     int x;
{
  if ((!!a) != (!!b))
    abort ();
}

f (x)
     int x;
{
  g (x * x);
}

main ()
{
  f (100);
  exit (0);
}

/* cp-out: warning: [^:]*: line 8, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 20, columns 2-6: identifier "exit" not declared */
