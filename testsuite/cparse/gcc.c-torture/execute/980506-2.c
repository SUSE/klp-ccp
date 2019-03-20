static void *self(void *p){ return p; }

int
f()
{
  struct { int i; } s, *sp;
  int *ip = &s.i;

  s.i = 1;
  sp = self(&s);
  
  *ip = 0;
  return sp->i+1;
}

main()
{
  if (f () != 1)
    abort ();
  else
    exit (0);
}

/* cp-out: warning: [^:]*: line 19, column 4: identifier "abort" not declared */
/* cp-out: warning: [^:]*: line 21, column 4: identifier "exit" not declared */
