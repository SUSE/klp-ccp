/* { dg-skip-if "requires alloca" { ! alloca } { "-O0" } { "" } } */
g (x, y)
{
  if (x != 3)
    abort ();
}

static inline
f (int i)
{
  int *tmp;

  tmp = (int *) alloca (sizeof (i));
  *tmp = i;
  g (*tmp, 0);
}

main ()
{
  f (3);
  exit (0);
};

/* cp-out: warning: [^:]*: line 5, columns 4-9: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 13, columns 16-22: identifier "alloca" not declared */
/* cp-out: warning: [^:]*: line 21, columns 2-6: identifier "exit" not declared */
