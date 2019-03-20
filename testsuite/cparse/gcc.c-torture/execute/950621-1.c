struct s
{
  int a;
  int b;
  struct s *dummy;
};

f (struct s *sp)
{
  return sp && sp->a == -1 && sp->b == -1;
}

main ()
{
  struct s x;
  x.a = x.b = -1;
  if (f (&x) == 0)
    abort ();
  exit (0);
}

/* cp-out: warning: [^:]*: line 18, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 19, column 2: identifier "exit" not declared */
