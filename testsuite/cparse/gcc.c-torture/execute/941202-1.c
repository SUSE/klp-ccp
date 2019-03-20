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

/* cp-out: warning: [^:]*: line 5, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 13, column 16: identifier "alloca" not declared */
/* cp-out: warning: [^:]*: line 21, column 2: identifier "exit" not declared */
