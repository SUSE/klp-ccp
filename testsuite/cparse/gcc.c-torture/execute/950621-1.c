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

/* cp-out: warning: [^:]*: line 18, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 19, columns 2-6: identifier "exit" not declared */
